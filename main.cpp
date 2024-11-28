#include "NNalgorithm.hpp"
#include <curl/curl.h>
#include <zip.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <unistd.h>

namespace fs = std::filesystem;

// for handling JSON formatting
struct FormParams {

  std::ostringstream formParams;
  CURL *curl;
  int numParams = 0;

  FormParams(CURL *curl) : curl(curl) {}

  void addParam(std::string Name, std::string value) {

    if (numParams) {
      formParams << "&";
    }

    formParams << Name << "="
               << curl_easy_escape(curl, value.c_str(), value.length());

    numParams++;
  }

  
  std::string str() { return formParams.str(); }
};



CURLcode curlPOST(CURL *curl, std::string url, struct FormParams &params,
                  std::string &response);
CURLcode curlReq(CURL *curl, std::string url, std::string &response);

int alignSequences(const std::string &sequences,  std::ostream &phylipOut);

int main(int argc, char **args) {

  std::ostringstream sequences;

  if (argc < 4) {
    std::cout << "Please input at least 3 files in FASTA format" << std::endl;
    return -1;
  }

  // Read FASTA files
  for (int i = 1; i < argc; i++) {
    std::ifstream seq(args[i]);

    if (seq.is_open()) {
      sequences << std::string((std::istreambuf_iterator<char>(seq)),
                               (std::istreambuf_iterator<char>()));
    } else {
      std::cout << "Failed to open file, exiting." << std::endl;
    }
  }


  std::stringstream alignedSeq;
  // Submit sequences to clustal omega to align
  if (!alignSequences(sequences.str(), alignedSeq)) {
    return -1;
  }

  std::cout << phylipToNewick(alignedSeq) << std::endl;

  
}


// From a string of sequences in FASTA format
int alignSequences(const std::string &sequences,  std::ostream &phylipOut) {

  const std::string CLUSTAL_API_URL =
      "https://www.ebi.ac.uk/Tools/services/rest/clustalo";

  CURL *curl = curl_easy_init();

  if (!curl) {
    std::cout << "Couldn't init curl, aborting" << std::endl;
    return 0;
  }

  // submit msa job
  struct FormParams jobParams(curl);

  jobParams.addParam("email", "noahwzinn@gmail.com");
  jobParams.addParam("outfmt", "phylip");
  jobParams.addParam("sequence", sequences);

  std::cout << "Submitting multiple sequence alignment job" << std::endl;
  std::string jobID;

  // Submit the job

  CURLcode jobSubmit =
      curlPOST(curl, CLUSTAL_API_URL + "/run", jobParams, jobID);

  // Quit if theres a curl error submitting the job
  if (jobSubmit != CURLE_OK) {
    std::cout << "Error submitting job: CURLcode " << jobSubmit << ", aborting"
              << std::endl;
    return 0;
  }

  // Handle if we get an error from clustal omega
  if (jobID[0] == '<') {
    std::cout << "Error from clustal: " << std::endl << jobID << std::endl;

    return 0;
  }

  std::cout << "Submitted" << std::endl;
  // wait for job to finish

  CURLcode waiting;
  std::string jobStatus = "";

  while (jobStatus != "FINISHED") {
    std::cout << "waiting for results.." << std::endl;
    sleep(2);
    waiting = curlReq(curl, CLUSTAL_API_URL + "/status/" + jobID, jobStatus);

    if (waiting != CURLE_OK) {
      std::cout << "Curl error: " << waiting << std::endl;
      return 0;
    }
  }

  std::cout << "Downloading results" << std::endl;

  std::string zipFile;

  CURLcode download =
      curlReq(curl, CLUSTAL_API_URL + "/result/" + jobID + "/zip", zipFile);

  if (download != CURLE_OK) {
    std::cout << "Curl error: " << download << std::endl;
    return 0;
  }

  // Check if results folder exists, if not create it

  if (!fs::exists(fs::path("results/"))) {
    std::system("mkdir results");
  }


  zip_error_t error;
  zip_source_t *zipSource = zip_source_buffer_create(zipFile.c_str(), zipFile.length(), 0, &error);
  zip_t *archive = zip_open_from_source(zipSource, 0, &error);

  std::string filename = jobID + ".phylip";
  zip_file_t *phylipFile = zip_fopen(archive, filename.c_str(), 0);


  char *buf;

  while (zip_fread(phylipFile, buf, 1)) {
    phylipOut << *buf;
  }
  

  zip_fclose(phylipFile);
  zip_close(archive);
  curl_easy_cleanup(curl);

  return 1;
}

// Function for handling data from curl
size_t curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata) {

  std::ostringstream *data = (std::ostringstream *)userdata;

  for (int i = 0; i < nmemb; i++) {
    *data << ptr[i];
  }

  return size * nmemb;
}

// Perform a simple request, returning the curlcode and putting the response in
// the given string
CURLcode curlReq(CURL *curl, std::string url, std::string &response) {
  std::ostringstream result;
  curl_easy_reset(curl);

  CURLcode res;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  // Write function callback and data
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

  res = curl_easy_perform(curl);

  response = result.str();
  return res;
}

// Execute a POST request
CURLcode curlPOST(CURL *curl, std::string url, struct FormParams &params,
                  std::string &response) {
  // Set headers

  curl_easy_reset(curl);

  std::ostringstream result;

  // Set options
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, params.str().length());

  std::string data = params.str();
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

  // Write function callback and data
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

  CURLcode res;

  // Submit job

  res = curl_easy_perform(curl);

  response = result.str();

  return res;
}

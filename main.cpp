#include "NNalgorithm.hpp"
#include <curl/curl.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>




// for handling JSON formatting
struct FormParams {

  std::ostringstream formParams;
  CURL *curl;
  int numParams = 0;

  FormParams(CURL *curl) : curl(curl){}

  void addParam(std::string Name, std::string value) {

    if (numParams) {
      formParams << "&";
    }
    
    formParams << Name << "=" << curl_easy_escape(curl, value.c_str(), value.length());
    
    numParams++;
  }

  std::string str() { return formParams.str(); }
  
};


std::string curlPOST(CURL *curl, std::string url, struct FormParams &params);



int main(int argc, char **args) {

  const std::string CLUSTAL_API_URL =
      "https://www.ebi.ac.uk/Tools/services/rest/clustalo";

  std::ostringstream sequences;

  if (argc < 4) {
    std::cout << "Please input at least 3 files in FASTA format" << std::endl;
    return -1;
  }

  for (int i = 1; i < argc; i++) {
    std::ifstream seq(args[i]);

    if (seq.is_open()) {
      sequences << std::string((std::istreambuf_iterator<char>(seq)), (std::istreambuf_iterator<char>()));
    } else {
      std::cout << "Failed to open file, exiting." << std::endl; 
    }
  }

  CURL *curl = curl_easy_init();

  if (curl) {

    struct FormParams jobParams(curl);

    jobParams.addParam("email", "noahwzinn@gmail.com");
    jobParams.addParam("outfmt", "phylip");
    jobParams.addParam("sequence", sequences.str());

    std::cout << curlPOST(curl, CLUSTAL_API_URL + "/run", jobParams) << std::endl;

  } else {
    std::cout << "Couldn't init curl!" << std::endl;
  }
  curl_easy_cleanup(curl);
}


size_t curlWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata) {

  std::ostringstream *data = (std::ostringstream *)userdata;

  for (int i = 0; i < nmemb; i++) {
    *data << ptr[i];
  }

  return size * nmemb;
}


// Execute a POST request
std::string curlPOST(CURL *curl, std::string url, struct FormParams &params) {
  // Set headers

    struct curl_slist *headers = nullptr;

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

    if (res == CURLE_OK) {
      return result.str();
    } else {
      return "Error";
    }
}

from ete3 import Tree, TreeStyle
import sys
ts = TreeStyle()
ts.show_leaf_name = True

t = Tree(sys.argv[1])

t.show()

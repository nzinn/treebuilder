from ete3 import Tree, TreeStyle
import sys
ts = TreeStyle()
ts.show_leaf_name = True


t = Tree(sys.argv[1])


# If an outgroup is given, set it
if len(sys.argv) > 3:
    outgroup = t.get_descendants_by_name(sys.argv[2])
    t.set_outgroup(outgroup)
    filename = sys.argv[3]
else:
    filename = sys.argv[2]

t.render(filename, tree_style=ts, h=480, w=640)

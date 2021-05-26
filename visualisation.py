from igraph import *
from cairocffi import *

flg = int(input())  #flag shows whether we want to build a specific path
if flg == 1:    
    name1 = input()     #name of the topology file
    name2 = input()     #name of the routes file
    first = int(input())    #the starting point
    last = int(input())     #the endpoint
elif flg == 0:
    name1 = input()     #name of the ropology file
else:  #case of invalid input
    print("Invalid input")
    print("Enter 1 if you want to build the path between two nodes")
    print("Enter 0 if you just want to visualise the topology")
    exit(0)
words = name1.split('.')    #creating of output file name
out = words[0]
f1 = open(name1, 'r')   #opening of the topology file
cnt = 0
names = list()  #list of node labels
links = list()  #list of edges
for line in f1.readlines():     #reading from the file
    l = line.split(';')     #parsing
    if cnt != 0:
        st = int(l[0])
        nm = l[1]
        if nm != '':
            names.append(nm)
        if l[4] != '\n':
            fin = int(l[4])
            links.append(tuple([st, fin]))
    cnt += 1
f1.close()  #closing of the topology file
g = Graph(links)    #creation of the graph
g.vs["name"] = names    
layout = g.layout_kamada_kawai()
visual_style = {}   #style specifications
visual_style["vertex_size"] = 20
visual_style["vertex_color"] = "blue"
visual_style["vertex_label"] = g.vs["name"]
visual_style["layout"] = layout
visual_style["bbox"] = (800,800)
visual_style["margin"] = 50
visual_style["vertex_label_dist"] = 2
visual_style["edge_width"] = 3
visual_style["edge_color"] = list()
ln = len(links)
for k in range(ln):
    visual_style["edge_color"].append("black")
#if no path is needed the graph is plotted and the program terminates
if flg == 0:    
    out += ".png"   #creation of the output file name
    plot(g, out, **visual_style)
    exit(0)
f2 = open(name2, 'r')   #opening of the file containing routes
cnt = 0
path = list()   #main path
respath = list()    #reserve path if it exists
res = 0
for line in f2.readlines():     #reading of the file
    l = line.split(';')     #parsing
    if cnt != 0:
        if (int(l[0])) == first and (int(l[1])) == last and res == 1:
            respath = l[3].split(',')
            respath[0] = respath[0].replace('[', '')
            ln = len(respath)
            respath[ln - 1] = respath[ln - 1].replace(']', '')
            break
        if (int(l[0])) == first and (int(l[1])) == last and res == 0:
            path = l[3].split(',')
            path[0] = path[0].replace('[', '')
            ln = len(path)
            path[ln - 1] = path[ln - 1].replace(']', '')
            res += 1
    cnt += 1
f2.close()  #closing of the file
ln = len(path)
for k in range(ln):     #coloring edges
    if k != ln - 1:     #main path
        n1 = int(path[k])
        n2 = int(path[k + 1])
        change = g.get_eid(n1, n2)
        visual_style["edge_color"][change] = "red"
ln = len(respath)
if ln != 0:     #reserve path (if it exists)
    for k in range(ln):
        if k != ln - 1:
            n1 = int(respath[k])
            n2 = int(respath[k + 1])
            change = g.get_eid(n1, n2)
            visual_style["edge_color"][change] = "green"
#creation of the output file name
out += "_" + str(first) + "_" + str(last) + ".png"  
plot(g, out, **visual_style)    #plotting of the graph

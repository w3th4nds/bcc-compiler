#!/usr/bin/env python3
from dsplot.tree import BinaryTree
import webbrowser

# not in use

def make_graph():
  with open('AST_BFS.txt', 'r') as f:
    nodes = [l.strip() for l in f.readlines()]

  while (nodes[-1] == "NULL"): nodes = nodes[:-1] # remove trailing NULLs
  for i in range(len(nodes)): nodes[i] = None if nodes[i] == "NULL" else nodes[i]

  tree = BinaryTree(nodes=nodes)
  tree.plot(output_path='graph.png', border_color='#FFCE30', fill_color='#aec6cf')
  webbrowser.open('graph.png')

    
if __name__ == '__main__':
  make_graph()

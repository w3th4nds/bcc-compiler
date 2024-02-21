#!/usr/bin/env python3
from glob import glob
from dsplot.tree import BinaryTree
from dsplot.graph import Graph
import webbrowser

def create_program_graph(fname):
  with open(fname, 'r') as f:
    lines = [l.strip() for l in f.readlines()]

  separator = lines.index('Annotations:')
  nodes = [l.split(',')[:-1] for l in lines[:separator]]
  for i, node in enumerate(nodes):
    nodes[i] = [n.replace(' ', '\n') for n in node]
  nodes = {node[0] : node[1:] for node in nodes}

  annot = [l.replace(',', '').replace(' ', '\n').split('->') for l in lines[separator+1:]]
  annot = {a[0]:a[1] for a in annot}

  graph = Graph(nodes, directed=True, edges=annot)
  outname = f'{fname[:-4]}_graph.png'
  graph.plot(output_path=outname, orientation='TB', fill_color='#BAC1D2', border_color='#8578fa', shape='doublecircle')
  webbrowser.open(outname)

    
if __name__ == '__main__':
  # the data file is created by the compiler
  create_program_graph('AST_data.txt')

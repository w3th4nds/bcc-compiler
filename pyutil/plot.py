#!/usr/bin/env python3
from glob import glob
from dsplot.tree import BinaryTree
from dsplot.graph import Graph
import webbrowser

def create_program_graph(fname):
  print(f'[*] Creating graph for: {fname}')
  with open(fname, 'r') as f:
    lines = [l.strip() for l in f.readlines()]
  # formatting
  lines = [l.split(',')[:-1] for l in lines]
  for il, line in enumerate(lines):
    lines[il] = [n.replace(' ', '\n') for n in line]
  tree = {line[0]:line[1:] for line in lines}
  graph = Graph(tree, directed=True)
  outname = f'{fname[:-4]}_graph.png'
  graph.plot(output_path=outname, orientation='UD', border_color='#FFCE30', fill_color='#aec6cf')
  webbrowser.open(outname)

    
if __name__ == '__main__':
  # created by the compiler
  create_program_graph('AST_data.txt')

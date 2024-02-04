#!/usr/bin/env python3
from glob import glob
from dsplot.tree import BinaryTree
import webbrowser

def create_graph(fname):
  print(f'[*] Creating graph for: {fname}')
  with open(fname, 'r') as f:
    nodes = [l.strip() for l in f.readlines()]
  while (nodes[-1] == 'NULL'): nodes = nodes[:-1] # remove trailing NULLs
  for i in range(len(nodes)): nodes[i] = None if nodes[i] == 'NULL' else nodes[i]
  tree = BinaryTree(nodes=nodes)
  outname = f'{fname[:-4]}_graph.png'
  tree.plot(output_path=outname, border_color='#FFCE30', fill_color='#aec6cf')
  webbrowser.open(outname)
    
if __name__ == '__main__':
  ast_files = sorted(glob('AST_BFS_*'))
  for fname in ast_files:
    create_graph(fname)

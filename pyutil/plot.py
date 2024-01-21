#!/usr/bin/env python3
from dsplot.graph import Graph
import webbrowser

def make_graph():
  with open('AST_BFS.txt', 'r') as f:
    lines = [l.strip().split(',')[:-1] for l in f.readlines()]

  target = dict()
  for l in lines:
    target.update({l[0]: l[1:]})
  print(target)

  graph = Graph(target, directed=True)
  graph.plot(orientation='UD', fill_color='#aec6cf')
  webbrowser.open('graph.png')
    
if __name__ == '__main__':
  make_graph()

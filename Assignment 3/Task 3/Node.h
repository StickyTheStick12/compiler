#ifndef NODE_H
#define	NODE_H

#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

//using namespace std;


class Node {
public:
    int id, lineno;
    std::string type, value;
    std::list<Node*> children;
    Node(std::string t, std::string v, int l) : type(t), value(v), lineno(l){}
    Node()
    {
        type = "uninitialised";
        value = "uninitialised"; }   // Bison needs this.

    void print_tree(int depth=0) {
        for(int i=0; i<depth; i++)
            std::cout << "  ";
        std::cout << type << ":" << value << std::endl; //<< " @line: "<< lineno << endl;
        for(auto i=children.begin(); i!=children.end(); i++)
            (*i)->print_tree(depth+1);
    }

    void generate_tree() {
        std::ofstream outStream;
        char* filename = "tree.dot";
        outStream.open(filename);

        int count = 0;
        outStream << "digraph {" << std::endl;
        generate_tree_content(count, &outStream);
        outStream << "}" << std::endl;
        outStream.close();

        printf("\nBuilt a parse-tree at %s. Use 'make tree' to generate the pdf version.\n", filename);
    }

    void generate_tree_content(int &count, std::ofstream *outStream) {
        id = count++;
        *outStream << "n" << id << " [label=\"" << type << ":" << value << "\"];" << std::endl;

        for (auto i = children.begin(); i != children.end(); i++)
        {
            (*i)->generate_tree_content(count, outStream);
            *outStream << "n" << id << " -> n" << (*i)->id << std::endl;
        }
    }

};

#endif

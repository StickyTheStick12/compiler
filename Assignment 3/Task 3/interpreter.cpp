#include "interpreter.h"

Activation::Activation(BCMethod* method) {
    this->method = method;

    for(auto methodVar : method->variables)
        variables[methodVar] = 0;
}

Instruction* Activation::GetNextInstruction() {
    return method->GetInstruction(pc++);
}

void Activation::Jump(const std::string& block) {
    pc = method->GetPcValue(block);
}

Program* ReadFile(const std::string& filename)
{
    Program* program = new Program;
    BCMethod* currentMethod;
    MethodBlock* currentMethodBLock;
    std::ifstream file(filename);

    std::string line;
    while(getline(file, line))
    {
        if(line == "main:")
        {
            currentMethod = new BCMethod();
            currentMethodBLock = new MethodBlock();
            currentMethodBLock->name = "main";
            currentMethod->methodBlocks.push_back(currentMethodBLock);
        }
        else if(line.find('.') != std::string::npos && line.find(':') != std::string::npos) {
            line.pop_back();
            currentMethodBLock = new BCMethod();
            currentMethodBLock = new MethodBlock();
            currentMethodBLock->name = line;
            currentMethod->methodBlocks.push_back(currentMethodBLock);
            program->methods[line] = currentMethod;
        }
        else if(line.find(':') != std::string::npos) {
            currentMethodBLock = new MethodBlock();
            line.pop_back();
            currentMethodBLock->name = line;
            currentMethod->methodBlocks.push_back(currentMethodBLock);
        }
        else {
            Instruction* instruction = new Instruction();
            std::vector<std::string> nLine;

            int pos = 0;
            std::string token;
            while ((pos = line.find(' ')) != std::string::npos) {
                token = line.substr(0, pos);
                nLine.push_back(token);
                line.erase(0, pos + 1);
            }

            instruction->id = instructions[nLine[0].]
        }
    }
}





        } else {
            inst->id = instructions.at(lineSplit[0]);
            if (lineSplit.size() > 1) {
                inst->argument = lineSplit[1];
                if ((lineSplit[0] == "iload" || lineSplit[0] == "istore") && find(currentMethod->variables.begin(), currentMethod->variables.end(), lineSplit[1]) != currentMethod->variables.end()) {
                    currentMethod->variables.push_back(lineSplit[1]);
                }
            }
            currentMethodBlock->instructions.push_back(inst);
        }
    }
    return program;

}

void interpret(Program* program) {
    stack<Activation*> activationStack;
    stack<int> dataStack;
    Activation* currentActivation = new Activation(program->methods["main"]);
    int instructionId = -1;
    int v1, v2, v;
    while(instructionId != STOP) {
        Instruction* instruction = currentActivation->getNextInstruction();
        // cout << instruction->id << " " << instruction->argument << endl;
        instructionId = instruction->id;
        switch(instruction->id) {
            case ILOAD:
                dataStack.push(currentActivation->variables[instruction->argument]);
                break;
            case ICONST:
                dataStack.push(stoi(instruction->argument));
                break;
            case ISTORE:
                currentActivation->variables[instruction->argument] = dataStack.top();
                dataStack.pop();
                break;
            case IADD:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v1 + v2);
                break;
            case ISUB:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 - v1);
                break;
            case IMUL:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 * v1);
                break;
            case IDIV:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 / v1);
                break;
            case ILT:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 < v1);
                break;
            case IEQ:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 == v1);
                break;
            case IAND:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 * v1 != 0);
                break;
            case IOR:
                v1 = dataStack.top();
                dataStack.pop();
                v2 = dataStack.top();
                dataStack.pop();
                dataStack.push(v2 + v1 != 0);
                break;
            case INOT:
                v = dataStack.top();
                dataStack.pop();
                dataStack.push(v == 0);
                break;
            case GOTO:
                currentActivation->jump(instruction->argument);
                break;
            case IFFALSE:
                v = dataStack.top();
                dataStack.pop();
                if(v == 0)
                {
                    currentActivation->jump(instruction->argument);
                }
                break;
            case INVOKEVIRTUAL:
            {
                activationStack.push(currentActivation);
                BCMethod* callMethod = program->methods[instruction->argument];
                currentActivation = new Activation(callMethod);
                break;
            }
            case IRETURN:
                currentActivation = activationStack.top();
                activationStack.pop();
                break;
            case PRINT:
                v = dataStack.top();
                dataStack.pop();
                cout << v << endl;
                break;
            case STOP:
                break;
        }
    }

}

vector<string> split_string(string inp, string del) {
    vector<string> res;
    int str_pos = 0;
    while (str_pos != string::npos) {
        str_pos = inp.find(del, str_pos);
        string part = inp.substr(0, str_pos);
        inp.erase(0, str_pos + 1);
        if (str_pos == 0) continue;
        res.push_back(part);
    }
    return res;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Too few arguments" << endl;
        return 0;
    }
    Program* program = readFile(argv[1]);
    interpret(program);
    return 0;
}

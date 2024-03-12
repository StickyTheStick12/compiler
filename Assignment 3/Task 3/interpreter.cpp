#include "interpreter.h"

Activation::Activation(BCMethod* method) {
    this->method = method;

    for(auto& methodVar : method->variables)
        variables[methodVar] = 0;
}

Instruction* Activation::GetNextInstruction() {
    return method->GetInstruction(pc++);
}

void Activation::Jump(const std::string& block) {
    pc = method->GetPcValue(block);
}

std::vector<std::string> split_string(std::string input) {
    std::vector<std::string> res;
    int str_pos = 0;
    while (str_pos != std::string::npos) {
        str_pos = input.find(' ', str_pos);
        std::string part = input.substr(0, str_pos);
        input.erase(0, str_pos + 1);
        if (str_pos == 0) continue;
        res.push_back(part);
    }
    return res;
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
            program->methods["main"] = currentMethod;
        }
        else if(line.find('.') != std::string::npos && line.find(':') != std::string::npos) {
            line.pop_back();
            line = line.substr(line.find('.') + 1);
            currentMethod = new BCMethod();
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

            nLine = split_string(line);
            instruction->id = instructions.at(nLine[0]);
            if(nLine.size() > 1)
            {
                instruction->argument = nLine[1];

                if ((nLine[0] == "iload" || nLine[0] == "istore") && find(currentMethod->variables.begin(), currentMethod->variables.end(), nLine[1]) != currentMethod->variables.end()) {
                    currentMethod->variables.push_back(nLine[1]);
                }
            }
            currentMethodBLock->instructions.push_back(instruction);
        }
    }

    return program;
}


void Interpret(Program* program)
{
    std::stack<Activation*> activationStack;
    std::stack<int> dataStack;
    Activation* currentAct = new Activation(program->methods["main"]);
    int instructionId = -1;
    int ra;
    int rb;

    while(instructionId != STOP)
    {
        Instruction* instruction = currentAct->GetNextInstruction();
        instructionId = instruction->id;

        switch (instruction->id) {
            case ILOAD:
                dataStack.push(currentAct->variables[instruction->argument]);
                break;
            case ICONST:
                dataStack.push(stoi(instruction->argument));
                break;
            case ISTORE:
                currentAct->variables[instruction->argument] = dataStack.top();
                dataStack.pop();
                break;
            case IADD:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(ra + rb);
                break;
            case ISUB:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(rb - ra);
                break;
            case IMUL:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(rb * ra);
                break;
            case IDIV:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(rb / ra);
                break;
            case ILT:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(rb < ra);
                break;
            case IEQ:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(rb == ra);
                break;
            case IAND:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(ra * rb != 0);
                break;
            case IOR:
                ra = dataStack.top();
                dataStack.pop();
                rb = dataStack.top();
                dataStack.pop();
                dataStack.push(ra + rb != 0);
                break;
            case INOT:
                ra = dataStack.top();
                dataStack.pop();
                dataStack.push(ra == 0);
                break;
            case GOTO:
                currentAct->Jump(instruction->argument);
                break;
            case IFFALSE:
                ra = dataStack.top();
                dataStack.pop();
                if(ra == 0)
                    currentAct->Jump(instruction->argument);
            break;
            case INVOKEVIRTUAL: {
                activationStack.push(currentAct);
                BCMethod *callMethod = program->methods[instruction->argument];
                currentAct = new Activation(callMethod);
                break;
            }
            case IRETURN:
                currentAct = activationStack.top();
                activationStack.pop();
                break;
            case PRINT:
                ra = dataStack.top();
                dataStack.pop();
                break;
            case STOP:
                break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Too few arguments" << std::endl;
        return 0;
    }

    Program* program = ReadFile(argv[1]);
    Interpret(program);
    return 0;
}

import re

with open('project.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace Grid(10) with Grid(15)
content = re.sub(r'Grid\(int size = 10\)', 'Grid(int size = 15)', content)
content = re.sub(r'grid = Grid\(10\);', 'grid = Grid(15);', content)
content = re.sub(r'const int SIZE = 10;', 'const int SIZE = 15;', content)

match_sg = re.search(r'void startGraphics\(\)', content)
match_main = re.search(r'int main\(\)', content)

if match_sg and match_main:
    # Find the closing brace of class Simulation which is just before main
    prefix = content[:match_sg.start()]
    suffix = content[match_main.start():]
    # We need to include '};\n\n' before main
    suffix = '};\n\n' + suffix
    
    with open('new_graphics.txt', 'r', encoding='utf-8') as f:
        new_sg = f.read()
    
    with open('project.cpp', 'w', encoding='utf-8') as f:
        f.write(prefix + new_sg + '\n' + suffix)
    print("Rewritten successfully.")
else:
    print("Could not find startGraphics or main")

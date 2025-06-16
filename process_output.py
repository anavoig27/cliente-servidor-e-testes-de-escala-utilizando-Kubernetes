import matplotlib.pyplot as plt

def read(): 
    data = []

    with open('output1.txt', 'r') as file:
        lines = []
        
        j = 1

        for line in file.readlines():
            if line.startswith('Total elapsed time'):
                lines.append(line)
            if line.startswith("Configurando"):
                
                if lines != []:
                    data = process(lines=lines)

                    i = line.split(" ")[-1].strip()
                    
                    plot(f'plot_{j}.png',  data)
                    j = i

                    lines = []
        data = process(lines=lines)

        
                    
        plot(f'plotcpp_{j}.png',  data)

        lines = []

def process(lines : list):
    config = {
        i : [] for i in range(2, 11, 2)
    }
    for i in range(len(lines)):    
        string = lines[i]
        number = string.strip().split(" ")[-1]
    
        if number[0] == ".":
            number = "0"+number
    
        config[(i // 10 + 1) * 2].append(float(number))

    return config
        

def plot(filename,  data):
    plt.figure(figsize=(10, 6))

    for key, values in data.items():
        plt.plot(range(len(values)), values, label=f'{key} Servidores', marker='o')

    plt.xlabel('Clientes')
    plt.ylabel('Tempo(s)')
    plt.title('Configurações de Servidores e Clientes')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()
    

read()

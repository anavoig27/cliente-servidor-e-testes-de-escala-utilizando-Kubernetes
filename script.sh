SERVER_HOST="chat-server"
SERVER_PORT=12345
JOB_TEMPLATE="client-job-template.yaml"


for num_msg in 1 10 100; do
    echo "Configurando numero de mensagens como $num_msg"
    

    for servidores in {2..10..2}; do
        kubectl scale deployment chat-server --replicas=0
    
        echo "Deploying $servidores servidor(es)..."
    
        kubectl scale deployment chat-server --replicas=$servidores
        kubectl wait --for=condition=available --timeout=150s deployment/chat-server
    
        for clientes in {10..100..10}; do
            echo "Rodando teste com $clientes cliente(s) para $servidores servidor(es)..."
    
            kubectl delete job client-job --ignore-not-found
            
            NUM_CLIENTS=$clientes NUM_MSG=$num_msg envsubst < $JOB_TEMPLATE | kubectl apply -f -
            kubectl wait --for=condition=complete --timeout=600s job/client-job
    
            
            echo "Processando logs dos clientes..."

    
            # Inicializa contadores
            total_sent=0
            total_received=0
            total_elapsed="0.0"
    
            # Pega todos os pods do job
            pods=$(kubectl get pods -l job-name=client-job -o jsonpath='{.items[*].metadata.name}')
    
            for pod in $pods; do
                # Pega os logs
                logs=$(kubectl logs $pod)
    
                # Extrai valores via grep + regex simples (ajuste conforme saída real)
                sent=$(echo "$logs" | grep -oP 'Total sent: \K\d+')
                received=$(echo "$logs" | grep -oP 'Total received: \K\d+')
                elapsed=$(echo "$logs" | grep -oP 'Elapsed time: \K[\d\.]+')
    
                # Acumula (cuidado com empty strings)
                sent=${sent:-0}
                received=${received:-0}
                elapsed=${elapsed:-0.0}
                
                total_sent=$((total_sent + sent))
                total_received=$((total_received + received))
                # soma elapsed como float
                total_elapsed=$(echo "$total_elapsed + $elapsed" | bc)
            done
    
            echo "Resultados agregados para $clientes clientes:"
            echo "Total sent: $total_sent"
            echo "Total received: $total_received"
            echo "Total elapsed time (s): $total_elapsed"
    
            echo "-------------------------------------------------------------"
        done
    done
done
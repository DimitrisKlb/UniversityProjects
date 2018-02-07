echo  "> Usage: ./run_tests N"
echo -e "> N: number of clients to connect to the server \n"

for((i=0 ; i < $1 ; i++))
do 
	./client random_orders &  
done
wait $(jobs -p)

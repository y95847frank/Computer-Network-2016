all :
	gcc -std=c99 sender.c -o sender
	gcc -std=c99 agent.c -o agent
	gcc -std=c99 receiver.c -o receiver

clean:
	rm sender
	rm receiver
	rm agent

sender_:
	./sender 127.0.0.1 7891 n.in

agent_:
	./agent 127.0.0.1 7891 0.2

receiver_:
	./receiver 127.0.0.1 7891 k.out

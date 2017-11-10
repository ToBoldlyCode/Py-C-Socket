#!/usr/bin/python
'''
###############################################
Sockets

File: server.py
Usage: python server.py <port>

Description: Starts a TCP connection to exchange messages with clients   connected to the same port.
			 Python modified from my IRC chatbot.
			 C modified from my CS344 encryption/decryption program.
#################################################		
'''

import socket
import sys

#True to print test statements
TEST = False
#10 char client name + ": " + 500 char + terminator
MAX_CHAR = 513
EXIT_MSG = "\quit"

#Set server values
'''
@connect(): tries to bind to a port and listen
@get_message(): prints messages over socket
@send_message(): sends messages over socket
'''
class server:
	'''
	@serverPort: port number
	@serverSocket: new TCP socket
	@connectionSocket: socket object for transmitting data
	@addr: client address
	'''
	def __init__(self):
		#set 2nd argument as port
		self.serverPort = int(sys.argv[1])
		self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.connectionSocket = None
		self.addr = None

	#define server functions
	'''	
	Startup - Bind to port and listen
	'''
	def connect(self):
		try:
			#try to bind to the given port
			self.serverSocket.bind(('',self.serverPort))
		#break if port is in use/illegal	
		except Exception as e:
			if TEST:
				print(e)
			print("Port connection denied. Try another one.")
			sys.exit()
		
		#listen for messages on socket
		self.serverSocket.listen(1)
		self.connectionSocket, self.addr = self.serverSocket.accept()
		
		if TEST:
			print('Server connected on ' + socket.gethostname())
			print('Listening for client on ' + str(self.serverPort))
	
	'''
	Receive Message
	@message: message from client
	'''
	def get_message(self):
			while True:
				message = self.connectionSocket.recv(MAX_CHAR)
				print(message)
				
				#if client exits, wait for a new client to connect
				if (message[-5:] == EXIT_MSG):
					print("Client has quit. Waiting for new connection.")
					self.connectionSocket.close()
					self.connectionSocket, self.addr = self.serverSocket.accept()
				#otherwise we can respond to the message
				else:
					self.send_message()
			
	'''
	Send Message
	@message: input to send to client
	'''
	def send_message(self):
			message = raw_input('Server: ')[:MAX_CHAR]
			self.connectionSocket.send('Server: ' + message)
			
			#if our message is \quit, exit
			if (message[-5:] == EXIT_MSG):
				sys.exit()
			else:
				return

def main():
	chat_server = server()
	chat_server.connect()
	chat_server.get_message()
	
main()
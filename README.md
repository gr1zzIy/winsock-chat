# Simple Chat Application

A simple multi-client chat application built with C++ and Winsock2, featuring real-time messaging and nickname support.

## Features

- Multi-client support - Multiple users can connect simultaneously
- Nickname system - Each user chooses a unique nickname
- Real-time messaging - Instant message broadcasting to all connected clients
- User join/leave notifications - Automatic notifications when users enter or leave the chat
- Cross-client communication - Messages are visible to all connected clients
- Clean disconnect handling - Proper cleanup when users leave the chat

## Requirements

- Windows OS (uses Winsock2 API)
- C++ compiler (Visual Studio, MinGW, or similar)
- Winsock2 library (included with Windows)

## Building the Application

### Using Visual Studio
1. Create a new C++ project
2. Add server.cpp and client.cpp to the project
3. Build both files separately

## Usage

### Starting the Server
1. Run the server application first:
./server.exe
2. The server will start on port 54000 and wait for connections:
Server started on port 54000. Waiting for clients...

### Connecting Clients
1. Run the client application:
./client.exe
2. Enter your nickname when prompted:
Enter your nickname: Alice
3. Start chatting! The client will connect to localhost (127.0.0.1) on port 54000.

### Chat Commands
- Type normal messages to send to all users
- Type quit or exit to leave the chat gracefully
- Empty messages are ignored

## Example Session

### Server Output

<img width="480" height="155" alt="Знімок екрана 2025-10-05 182352" src="https://github.com/user-attachments/assets/93a09ed7-7394-4542-a8f8-e4f207671452" />

### Client Output (Alice)

<img width="413" height="232" alt="client-Alice" src="https://github.com/user-attachments/assets/7ed9472b-2b91-441c-a92e-080e7acf6d96" />

### Client Output (Oleksii)

<img width="422" height="194" alt="client-Oleksii" src="https://github.com/user-attachments/assets/95c86844-b501-4131-ada0-c18d8959335d" />

## Network Configuration

- Default Port: 54000
- Default Address: 127.0.0.1 (localhost)

To change the server address or port, modify the DEFAULT_ADDR and DEFAULT_PORT constants in the source code.

## Technical Details

### Server Features
- Handles multiple clients using threads
- Uses mutex locks for thread-safe client management
- Automatic client cleanup on disconnect
- Broadcasts messages to all connected clients except sender

### Client Features
- Separate thread for receiving messages
- Real-time message display
- Proper connection shutdown
- User-friendly interface with input prompts

## Limitations

- No message history persistence
- No user authentication
- No private messaging
- No room/channel support
- Windows-only due to Winsock2 dependency

## Future Enhancements

Potential improvements for this chat application:
- [ ] Message encryption
- [ ] User authentication
- [ ] Private messaging
- [ ] Chat rooms/channels
- [ ] Message history
- [ ] File sharing
- [ ] Cross-platform support (Linux/macOS)

## Troubleshooting

### Common Issues

1. "Address already in use": Wait a few minutes for the port to become available or change the port number.

2. Connection refused: Ensure the server is running before starting clients.

3. Winsock errors: Verify that Winsock2 is properly initialized.

### Firewall Configuration
You may need to allow the applications through Windows Firewall when running on different machines.

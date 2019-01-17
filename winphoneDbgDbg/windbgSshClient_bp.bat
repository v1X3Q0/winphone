bp SshClient!WinSshStartup
bp SshClient!WinSshSocket
bp SshClient!WinSshConnectOnSocket
bp WinSshQuerySocketProperty
bp SshClient!WinSshEndpointRead
bp SshClient + 0x00012110
:: WinSsh::ClientSocket::SocketWrite
bp SshClient + 0x0001201A
:: WinSsh::ClientSocket::SocketRead
bp SshClient + 0x000246C0
:: WinSsh::DispatcherPort::WriteSocket
bp SshClient + 0x000245C0
:: WinSsh::DispatcherPort::ReadSocket
bp SshClient + 0x00011395
:: WinSsh::ClientSocket::Connect
bp SshClient + 0x000106FB
:: WinSsh::Client::CreateSocket
bp SshClient + 0x00015799
:: WinSsh::ClientEndpoint::ReadRequest::ReadRequest
bp SshClient + 0x00014D4D
:: WinSsh::ClientEndpoint::ReadWorker
bp SshClient + 0x00024260
:: WinSsh::DispatcherPort::ReadFile
bp SshClient + 0x000156DB
:: WinSsh::ClientEndpoint::CompleteWrites
bp SshClient + 0x00015757
:: WinSsh::ClientEndpoint::WriteRequest::WriteRequest
bp SshClient + 0x000243A0
:: WinSsh::DispatcherPort::WriteFile
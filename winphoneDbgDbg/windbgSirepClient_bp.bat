bp SirepClient + 0x00013250
:: CSirepProtocol3 SirepConnect
:: bp SirepClient + 0x0001330F
:: close on timeout
bp SirepClient + 0x000133B7
:: bind socket
:: bp SirepClient + 0x00015D74
:: is protocol disabled in registry
bp SirepClient + 0x00015DFC
:: establish protocol connection
:: bp SirepClient + 0x00015EAF
:: second protocol connection establish
:: bp SirepClient + 0x00015BDD
:: First EstablishProtocolConnection variable call
:: bp SirepClient + 0x00015BF2
:: second one
:: bp SirepClient + 0x00015C0C
:: third one
:: bp SirepClient + 0x00015C23
:: fourth one
:: bp SirepClient + 0x00015C46
:: fifth one
:: bp SirepClient + 0x0001113A
:: srpclnt2 connect desktoppipe
bp SirepClient + 0x0001C0E6
:: connect in establish stuff
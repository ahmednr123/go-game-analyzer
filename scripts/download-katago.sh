
#KataGo:
#Windows CPU: https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-eigenavx2-windows-x64.zip
#Windows GPU: https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-opencl-windows-x64.zip
#Linux CPU: https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-eigenavx2-linux-x64.zip
#Linux GPU: https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-opencl-linux-x64.zip

#main model: https://media.katagotraining.org/uploaded/networks/models/kata1/kata1-b18c384nbt-s9996604416-d4316597426.bin.gz
#human model: https://media.katagotraining.org/uploaded/networks/models_extra/b18c384nbt-humanv0.bin.gz

wget https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-eigenavx2-windows-x64.zip
wget https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-opencl-windows-x64.zip
wget https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-eigenavx2-linux-x64.zip
wget https://github.com/lightvector/KataGo/releases/download/v1.16.4/katago-v1.16.4-opencl-linux-x64.zip

mkdir katago-windows-cpu
mkdir katago-windows-gpu
mkdir katago-linux-cpu
mkdir katago-linux-gpu

unzip katago-v1.16.4-eigenavx2-windows-x64.zip katago -d katago-windows-cpu
unzip katago-v1.16.4-opencl-windows-x64.zip katago -d katago-windows-cpu
unzip katago-v1.16.4-eigenavx2-linux-x64.zip katago -d katago-linux-cpu
unzip katago-v1.16.4-opencl-linux-x64.zip katago -d katago-linux-gpu

wget https://media.katagotraining.org/uploaded/networks/models/kata1/kata1-b18c384nbt-s9996604416-d4316597426.bin.gz

mkdir assets/KataGo/models
mv kata1-b18c384nbt-s9996604416-d4316597426.bin.gz assets/KataGo/models/

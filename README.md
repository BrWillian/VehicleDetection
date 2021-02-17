# VehicleDetection

VehicleDetection é uma biblioteca c++ que utiliza a interface da [darknet](https://bitbucket.org/vizentecpdi/darknet/src/vizentec/) para classificação de veiculos . 

## Instalação

Para instalação é necessario cmake-3.16.0, libudev-dev, openjdk-8-jdk, g++, wget, após todas Dependências instaladas, execute o prepare.sh, e em seguida build.sh.

```bash
chmod +x prepare.sh build.sh
./prepare.sh && ./build.sh
```

Após a instalação é necessário apenas carregar a biblioteca ```libvh.so``` contida na pasta```out```.

## Modo de Utilização

Para utilização a biblioteca possui interface para outras linguagens.
exemplo:

```Java
long handle = IDetector.INSTANCE.C_CreateVehicleDetection(cfg_path, weights_path, class_path);
System.out.println(IDetector.INSTANCE.C_SingleDetection(handle, imgData, imgSize, thres));
```

#### Legenda
```bash
 cfg_path (Caminho do arquivo de configuração de rede.)
 weights_path (Caminho do arquivo de pesos da rede.)
 class_path (Caminho do arquivo de classes.)
```

## Chamadas de função
Para chamadas em outras linguagens (Python, Java ...)
```Java
C_CreateVehicleDetectionEnv();
C_CreateVehicleDetection(string cfg_path, string weights_path, string class_path);
C_SingleDetection(Handle, Byte[], Byte[].length, float threshold);
C_FreeResult(handle);
```
Para chamadas nativas (C++)
```
VehicleDetector(const char* cfg_path, const char* weight_path, const char* class_path);
SingleDetection(cv::Mat, float thresh);

```

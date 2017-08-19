# Intelligent Video Analytics Pipeline for Security Camera (CCTV) Monitoring

Vagrantfile and Vagrant_setup.sh create an Ubuntu 16.04 box with the required packages

### Customizations...
#### Vagrantfile:

Change (or remove) the bridged network adaptor to your requirement

Change (or remove) the mount of the shared directory 

#### Vagrant_setup.sh:

Change the timezone setting


### Setup third party libraries
```
cd third_party
tar -xf rapidjson.tar
sudo mv rapidjson /usr/local/include
tar -xf websocketpp.tar
sudo mv websocketpp /usr/local/include
```


environment.yml - used to create conda env


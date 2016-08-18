Open Source Overview
============================
IVP
Version 2.3
The objective of the Integrated V2I Prototype (IVP) project is to identify, develop, implement, 
test, document and deploy a roadside prototype system that supports an integrated, interoperable 
deployment of multiple V2I safety, mobility, and environmental applications. 

Please see the IVP - Application Description.docx for more information.


License information
-------------------
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied. See the License for the specific language governing
permissions and limitations under the License.


System Requirements
-------------------------
The IVP software can run on most Linux based computers with 
Pentium core processers, with at least two gigabits of RAM and at least 64GB of drive space.
Performance of the software will be based on the computing power and available RAM in 
the system.  The project was developed and tested on a machine 
with a core i3 processor, 4GB of memory, 64GB of hard drive space, running Ubuntu 14.04LTS.

The IVP software application was developed using c and c++ and requires the following packages installed via apt-get:

cmake
gcc-4.8
g++-4.8
libboost1.55-dev
libboost-thread1.55-dev
libboost-regex1.55-dev
libboost-log1.55-dev
libboost-program-options1.55-dev
libboost1.55-all-dev
libxerces-c-dev
libcurl4-openssl-dev
libsnmp-dev
libmysqlclient-dev
libjsoncpp-dev
uuid-dev
libusb-dev libusb-1.0.0-dev
libftdi-dev
swig
liboctave-dev
gpsd libgps-dev
portaudio19-dev libsndfile-dev
libglib2.0-dev libglibmm-2.4-dev
libpcre3-dev
libsigc++-2.0-dev
libxml++2.6-dev
libxml2-dev
liblzma-dev

The following must be compiled and installed from the included source in TMX_OAM/Externals directory.

Mysql-connector-c__-1.1.3
	Run the following from your TMX_OAM/Externals directory
	tar xzf mysql-connector-c++-1.1.3.tar.gz
		cd mysql-connector-c++-1.1.3
		cmake .
		make
		sudo make install
		cd ..
		rm -rf mysql-connector-c++-1.1.3

J2735r41
	Run the following from your TMX-OAM/Externals directory
	./setupAsnJ2735_r41.sh


Documentation
-------------
Please refer to the IVP-Application Description.docx document for information on compiling and depolying IVP.


#script from Dmitri Konstantinov for vecgeom installation with vector ("vc") backend

#!/bin/bash

set -e

# Install Vc
VC_VERSION=1.4.3
wget https://lcgpackages.web.cern.ch/tarFiles/sources/Vc-${VC_VERSION}.tar.gz
tar xvf Vc-${VC_VERSION}.tar.gz
mkdir build_vc && cd build_vc
cmake -DCMAKE_INSTALL_PREFIX=../install/Vc/${VC_VERSION}/ -DCMAKE_BUILD_TYPE=Release ../Vc-${VC_VERSION}
make -j$(nproc) install

# Install VecCore
VECCORE_VERSION=0.8.0
wget https://lcgpackages.web.cern.ch/tarFiles/sources/VecCore-${VECCORE_VERSION}.tar.gz
tar xvf VecCore-${VECCORE_VERSION}.tar.gz
mkdir build_veccore && cd build_veccore
cmake -DCMAKE_INSTALL_PREFIX=../install/veccore/${VECCORE_VERSION}/ -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release ../veccore-${VECCORE_VERSION}
make -j$(nproc) install
# custmize the export
export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:/path-to/install/veccore/${VECCORE_VERSION}/:/path-to/install/Vc/${VC_VERSION}/

# Install VecGeom
VECGEOM_VERSION=1.2.1
wget https://lcgpackages.web.cern.ch/tarFiles/sources/VecGeom-v${VECGEOM_VERSION}.tar.gz
tar xvf VecGeom-v${VECGEOM_VERSION}.tar.gz
mkdir build_vecgeom && cd build_vecgeom
cmake ../VecGeom-v${VECGEOM_VERSION} \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=../install/VecGeom/${VECGEOM_VERSION}/ \
-DGEANT4=OFF \
-DUSOLIDS=ON \
-DUSOLIDS_VECGEOM=ON \
-DROOT=OFF \
-DCTEST=OFF \
-DBUILTIN_VECCORE=OFF \
-DBACKEND=Vc \
-DVECGEOM_BACKEND=Vc
make -j$(nproc) install

echo "Installation complete!"

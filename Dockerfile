FROM ubuntu
WORKDIR /app
COPY . .
RUN apt-get update
RUN apt-get -y install git
RUN apt-get -y install python3
RUN apt-get -y install python3-pip
RUN pip install conan --break-system-packages
RUN conan profile detect --force
RUN conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True -s compiler.cppstd=gnu20
RUN pip install conan --break-system-packages
RUN git submodule update --init --recursive
RUN cmake . -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake; cmake --build build

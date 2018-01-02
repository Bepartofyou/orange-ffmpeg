# ffmpeg static/shared libs with fdk-aac and x264

## env and src

* OS: centos 6.7 (yum install -y yasm nasm gcc gcc-c++ git cmake)
* ffmpeg branch: release/3.4
* fdk-aac  https://github.com/mstorsjo/fdk-aac.git
* x264  https://github.com/mirror/x264.git

## static build

* fkd-aac
    * ./autogen.sh
    * ./configure --prefix=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/static/fdk-aac --enable-static --disable-shared
    * make -j32
    * make install
    * make distclean
* x264
    * ./configure --prefix=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/static/x264 --enable-static --disable-shared

            Found no assembler
            Minimum version is nasm-2.13
            If you really want to compile without asm, configure with --disable-asm

            Solution:
            wget http://repo.or.cz/nasm.git/snapshot/b84ac822499b367566d66266bf6a4c41f257248a.zip
            unzip b84ac822499b367566d66266bf6a4c41f257248a.zip
            cd nasm-b84ac82
            ./autogen.sh 
            ./configure
            make -j32
            sudo make install


    * make -j32
    * make install
    * make distclean

* ffmpeg
    * export PKG_CONFIG_PATH=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/static/fdk-aac/lib/pkgconfig/:/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/static/x264/lib/pkgconfig/:$PKG_CONFIG_PATH
    * ./configure --prefix=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/static/ffmpeg --enable-static --disable-shared --enable-version3 --enable-gpl --enable-nonfree --enable-libfdk-aac --enable-libx264 
    * make -j32
    * make install
    * make distclean


## shared build

* fkd-aac
    * ./autogen.sh
    * ./configure --prefix=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/shared/fdk-aac --enable-shared --disable-static
    * make -j32
    * make install
    * make distclean
* x264
    * ./configure --prefix=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/shared/x264 --enable-shared --disable-static
    * make -j32
    * make install
    * make distclean

* ffmpeg
    * export PKG_CONFIG_PATH=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/shared/fdk-aac/lib/pkgconfig/:/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/shared/x264/lib/pkgconfig/:$PKG_CONFIG_PATH
    * ./configure --prefix=/home/xxxxxxxx/orange-ffmpeg/ffmpeg-libs/shared/ffmpeg --enable-shared --disable-static --enable-version3 --enable-gpl --enable-nonfree --enable-libfdk-aac --enable-libx264 
    * make -j32
    * make install
    * make distclean
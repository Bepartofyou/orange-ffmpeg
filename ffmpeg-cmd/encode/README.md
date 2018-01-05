## ffmpeg encoders

Just show encoder using, you can get detail infomation in http://ffmpeg.org/ffmpeg-all.html by searching corresponding encoder key words.

-------------------------

* loop   `单张图片合成视频,片头片尾`

      ../../ffmpeg-libs/static/ffmpeg/bin/ffmpeg -loop 1 -t 10 -i ./resource/weixin.png -pix_fmt yuv420p -vcodec libx264 -r 25 -g 25 -y weixin.mp4

*

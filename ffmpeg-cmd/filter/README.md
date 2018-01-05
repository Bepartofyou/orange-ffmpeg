## ffmpeg filters

Just show filter using, you can get detail infomation in http://ffmpeg.org/ffmpeg-all.html by searching corresponding filter key words.

-------------------------

* border   `图片添加边框`

      ../../ffmpeg-libs/static/ffmpeg/bin/ffmpeg  -i ./border/front.png  -filter_complex "crop=in_w-2*4:in_h-2*4[tmp];[tmp]pad=width=in_w+2*4:height=in_h+2*4:x=4:y=4:color=red" -y border.png
    * 加上crop是为了和原始图片尺寸保持一致
    * pad种的width和height就是原始图片尺寸

* grey   `图片变灰`

      ../../ffmpeg-libs/static/ffmpeg/bin/ffmpeg  -i ./resource/color.png  -filter_complex "colorchannelmixer=.3:.4:.3:0:.3:.4:.3:0:.3:.4:.3" -y grey.png

* fade `淡入淡出`

      ../../ffmpeg-libs/static/ffmpeg/bin/ffmpeg -i ./resource/input.mp4 -vf "fade=in:0:50,fade=out:450:50" -vcodec libx264 -r 25 -g 25 -y output.mp4
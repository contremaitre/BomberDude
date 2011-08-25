#!/bin/bash
echo usage : $0 orig dest orig_color dest_color
echo example : $0 tux tux 000000 ffffff will convert tux\{0,n\}.svg to tux\{0,n\}_\{down,up,left,right\}.png
width=50
i=0
while true
do
  in=$1$i".svg"
  out=$2$i".png"
  if [ ! -e $in ]; then
    break
  fi
  sed s/$3/$4/ $in > tmp.svg
  inkscape -h $width tmp.svg -e tmp.png
  cp tmp.png $2$i"_down.png"
  convert -rotate 90 tmp.png $2$i"_right".png
  convert -rotate 180 tmp.png $2$i"_up".png
  convert -rotate 270 tmp.png $2$i"_left".png
  echo ok
  rm tmp.png tmp.svg
  ((i++))
done



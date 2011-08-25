#!/bin/bash
echo usage : $0 origin_name dest_name orig_color dest_color
echo example : $0 tux_down  tux2_down 000000 ffffff will create tux_down\{0,n\}.svg \-\> tux2_down\{0,n\}.png
width=50
i=0
while true
do
  in=$1$i".svg"
  out=$2$i".png"
  if [ ! -e $in ]; then
    break
  fi
  echo $in > $out
  sed s/$3/$4/ $in > tmp.svg
  inkscape -h $width tmp.svg -e $out
  rm tmp.svg
  echo ok
  ((i++))
done



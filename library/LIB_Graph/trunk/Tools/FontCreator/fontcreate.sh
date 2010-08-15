#!/bin/sh
letter=a
pic_type=jpeg
#font_name=Courier-Bold
font_name=fixed
font_size=18
output_directory=${font_name}_${font_size}_font

############################################################################################################
list_letters="A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z 0 1 2 3 4 5 6 7 8 9  \. \\\ \/ \* \- \+ \= \( \) \& \! \, \; \?"
#list_letters="A .. F  \. \/ \x32"

############################################################################################################
echo create ouput directories
############################################################################################################
if [[  ! -e ${output_directory} ]]
then
	mkdir ${output_directory}
fi
if [[  ! -e ${output_directory}/${pic_type} ]]
then
	mkdir ${output_directory}/${pic_type}
fi
if [[  ! -e ${output_directory}/8 ]]
then
	mkdir ${output_directory}/8
fi
if [[  ! -e ${output_directory}/hex ]]
then
	mkdir ${output_directory}/hex
fi

############################################################################################################
echo create raw 8bpp binary file and convert it to hex
############################################################################################################
#function
creat_hex()
{
#convert -flop -rotate "180" -background white -fill black -font ${font_name}  -pointsize ${font_size} label:"$letter" ${output_directory}/${pic_type}/$letter_name.${pic_type}
#echo "${letter#\\}"|convert  -background white -fill black -font ${font_name}  -pointsize ${font_size} label:@- ${output_directory}/${pic_type}/$letter_name.${pic_type}
convert  -background white -fill black -font ${font_name}  -pointsize ${font_size} label:"${letter}" ${output_directory}/${pic_type}/$letter_name.${pic_type}
stream -map i -storage-type char ${output_directory}/${pic_type}/$letter_name.${pic_type} ${output_directory}/8/${letter_name}.8
witdh=`identify  -format "%w" ${output_directory}/${pic_type}/$letter_name.${pic_type}`
echo $witdh/1 \"0x%02X, \" \"\\n\" > format.txt
hexdump -v -f format.txt < ${output_directory}/8/${letter_name}.8  > ${output_directory}/hex/${letter_name}.hex

}
#Do it with list of letters
for letter in `echo $list_letters`
do
letter_name=`echo -n $letter | hexdump -e ' "0x%x"'| head -c 4 `
creat_hex
done
#special chars
letter_name=SPACE
letter=" "
creat_hex
rm format.txt

if [[  -e ${output_directory}/${font_name}_${font_size}.c ]]
then
	rm ${output_directory}/${font_name}_${font_size}.c
fi

############################################################################################################
echo create c file
############################################################################################################
#function
create_glyph ()
{
echo \/\* Letter: ${letter_code#\\} $letter\*\/                                                             >>${output_directory}/${font_name}_${font_size}.c
echo const unsigned char ${font_name}_${font_size}_Buff_${letter}\[\]\=\{                              >>${output_directory}/${font_name}_${font_size}.c
cat ${output_directory}/hex/${letter}.hex                                                              >>${output_directory}/${font_name}_${font_size}.c
echo \}\;                                                                                              >>${output_directory}/${font_name}_${font_size}.c
echo const Bitmap_t  ${font_name}_${font_size}_Glyphe_${letter} \= \{ eColor_8bpp ,                    >>${output_directory}/${font_name}_${font_size}.c
echo "                  "`identify  -format "%w" ${output_directory}/${pic_type}/$letter.${pic_type}`, >>${output_directory}/${font_name}_${font_size}.c
echo "                  "`identify  -format "%h" ${output_directory}/${pic_type}/$letter.${pic_type}`, >>${output_directory}/${font_name}_${font_size}.c
echo \(unsigned char\*\)${font_name}_${font_size}_Buff_${letter}                                       >>${output_directory}/${font_name}_${font_size}.c
echo "                  "\}\;                                                                          >>${output_directory}/${font_name}_${font_size}.c
echo ""                                                                                                >>${output_directory}/${font_name}_${font_size}.c
}
echo \#include \<stdint.h\>         >>${output_directory}/${font_name}_${font_size}.c
echo \#include \<LIB_Graph_Base.h\> >>${output_directory}/${font_name}_${font_size}.c
echo \#include \<LIB_Graph_Text.h\> >>${output_directory}/${font_name}_${font_size}.c
echo ""                             >>${output_directory}/${font_name}_${font_size}.c
echo ""                             >>${output_directory}/${font_name}_${font_size}.c
for letter_code in $list_letters
do
letter=`echo -n $letter_code | hexdump  -e ' "0x%x"'| head -c 4 `
create_glyph
done
letter=SPACE
letter_code="\" \""
create_glyph

##
echo "set the glyphe table"
##
echo const Bitmap_t const*${font_name}_${font_size}_Glyphes\[\] \= \{  >>${output_directory}/${font_name}_${font_size}.c
for letter_code in $list_letters
do
	letter=`echo -n $letter_code | hexdump  -e ' "0x%x"'| head -c 4 `
	echo "                  "\&${font_name}_${font_size}_Glyphe_${letter}, >>${output_directory}/${font_name}_${font_size}.c
done
letter=SPACE
echo "                  "\&${font_name}_${font_size}_Glyphe_${letter}, >>${output_directory}/${font_name}_${font_size}.c
echo \}\;                                                              >>${output_directory}/${font_name}_${font_size}.c

##
echo "create look up table, Warning take time :("
##
echo const unsigned char LUT_${font_name}_${font_size}\[\]\=\{         >>${output_directory}/${font_name}_${font_size}.c
for ((i=0;i<256;i++))
do
	count=0
	print0xff=1
	for letter_code in $list_letters
	do
		letter=`echo -n $letter_code | hexdump -e ' "0x%x"'| head -c 4`
		if((i==letter))
		then
			let print0xff=0
			echo -n $count", "                                            >>${output_directory}/${font_name}_${font_size}.c
		fi
		let "count+=1"
	done
	if((i==0x20))
	then
		echo -n $count", "                                                >>${output_directory}/${font_name}_${font_size}.c
	elif ((print0xff==1))
	then
		echo -n "0xff, "                                                  >>${output_directory}/${font_name}_${font_size}.c
	fi
done
echo \}\;                                                              >>${output_directory}/${font_name}_${font_size}.c
echo                                                                   >>${output_directory}/${font_name}_${font_size}.c
echo now set the font structure
echo ""                                                                >>${output_directory}/${font_name}_${font_size}.c
echo const Font_t ${font_name}_${font_size} \= \{                      >>${output_directory}/${font_name}_${font_size}.c
echo ${font_size},                                                     >>${output_directory}/${font_name}_${font_size}.c
echo LUT_${font_name}_${font_size},                                    >>${output_directory}/${font_name}_${font_size}.c
echo ${font_name}_${font_size}_Glyphes                                 >>${output_directory}/${font_name}_${font_size}.c
echo \}\;                                                              >>${output_directory}/${font_name}_${font_size}.c



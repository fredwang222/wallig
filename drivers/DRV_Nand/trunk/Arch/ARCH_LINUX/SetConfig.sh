grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ |grep CONFIG_DRV_NAND > config.txt

Get_Value()
{
	grep $1 config.txt | sed s/$1=// | sed s/\"//g	
}

echo '#define kNAND_SIZE '`Get_Value CONFIG_DRV_NAND_SIZE` > $CONFIG_PATH/DRV_Nand_CFG.h
echo '#define kNAND_FILE_NAME "'`Get_Value CONFIG_DRV_NAND_FILE`'"' >> $CONFIG_PATH/DRV_Nand_CFG.h

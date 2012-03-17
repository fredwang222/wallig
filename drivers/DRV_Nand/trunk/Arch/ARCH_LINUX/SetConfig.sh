grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ |grep CONFIG_DRV_NAND > config.txt

Get_Value()
{
	grep $1 config.txt | sed s/$1=// | sed s/\"//g	
}

echo '#define kNAND_FILE_NAME  "'`Get_Value CONFIG_DRV_NAND_FILE`'"' > $CONFIG_PATH/DRV_Nand_CFG.h
echo '#define kNAND_PAGE_SIZE   '`Get_Value CONFIG_DRV_NAND_PAGE_SIZE` >> $CONFIG_PATH/DRV_Nand_CFG.h
echo '#define kNAND_OOB_SIZE    '`Get_Value CONFIG_DRV_NAND_OOB_SIZE` >> $CONFIG_PATH/DRV_Nand_CFG.h
echo '#define kNAND_PAGE_COUNT  '`Get_Value CONFIG_DRV_NAND_PAGE_IN_BLOCK` >> $CONFIG_PATH/DRV_Nand_CFG.h
echo '#define kNAND_BLOCK_COUNT '`Get_Value CONFIG_DRV_NAND_BLOCK_COUNT` >> $CONFIG_PATH/DRV_Nand_CFG.h

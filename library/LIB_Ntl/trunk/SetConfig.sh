grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ |grep CONFIG_LIB_NTL > config.txt

Get_Value()
{
	grep $1 config.txt | sed s/$1=// | sed s/\"//g	
}

echo "Generate NTL Lib config"
echo '#define kNTL_BBM_OPR '`Get_Value CONFIG_LIB_NTL_BBM_OPR` > $CONFIG_PATH/LIB_Ntl_CFG.h
echo '#define kNTL_WL_OPR '`Get_Value CONFIG_LIB_NTL_WL_OPR` >> $CONFIG_PATH/LIB_Ntl_CFG.h

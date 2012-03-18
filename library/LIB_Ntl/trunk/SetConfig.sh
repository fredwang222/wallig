grep "#" -v $PRJ_CONFIG_FILE | grep -v ^$$ |grep CONFIG_LIB_NTL > config.txt

Get_Value()
{
	grep $1 config.txt | sed s/$1=// | sed s/\"//g	
}

echo "Generate NTL Lib config"
echo '#define kNTL_BBL_OPR '`Get_Value CONFIG_LIB_NTL_BBL_OPR` > $CONFIG_PATH/LIB_Ntl_CFG.h
echo '#define kNTL_WLL_OPR '`Get_Value CONFIG_LIB_NTL_WLL_OPR` >> $CONFIG_PATH/LIB_Ntl_CFG.h

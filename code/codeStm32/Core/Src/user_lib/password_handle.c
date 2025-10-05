#include "password_handle.h"
#include "app.h"

char rfid_code1[20]="44641";
char rfid_code2[20]="33146";
char rfid_code3[20]="77777"; //not use
char PASS_DEFAULT[16]="123";
char phone_number[20]="";
char passSim[20]="123";

bool simOpen=false;
char rfid_valid=RFID_IDE;
char kp=0;
uint32_t id=0;
char tam[20]="";

char set_key()
{

  id=0;
  kp=0;
  int timeOut=0;
  while(1)
  {
    if(rfid_valid!=RFID_IDE) break;
    esp32handle();

    kp=0;
    kp = read_key();
    if(kp) return kp;

    if(timeOut<10000) timeOut++;
    else
	{
    	kp='1';
		break;
	}

    /*
    if(sim.dataDoneFrame)
    {
    	//Mở cửa bằng sim: >open<
    	if(strstr(sim.arrRec,"open"))
    	{
    		simOpen=true;
    	}

    	sim.dataDoneFrame=false;
    	memset(sim.arrRec,0,sizeof(sim.arrRec));
    	break;
    }
    */

  }

}


void man_hinh_cho()
{
  _clear;
  kp=0;
  while(1)
  {
    lcd_str(1,1,"Nhan phim * de");
    lcd_str(2,1,"mo ban phim !");
    set_key();        //lien tuc kiem tra nut nhan
    if(kp=='*') break; //42 la ma ascii cua phim '*'
    if(rfid_valid==THE_DUNG) break;
    else if(rfid_valid==THE_SAI)
    {
      //memset(rfid_code,0,sizeof(rfid_code));
      _clear;
      lcd_str(1,1,"The rfid");
      lcd_str(2,1,"khong dung !");
      delay(1500);
      _clear;
    }
    else if(simOpen) break;
  }//while
}

void doi_pass()
{
	old_pass();
	_clear;
	lcd_str(1,1,"1-CLOSE");
	lcd_str(2,1,"2-CHANGE PASS");
}

void dong_cua()
{
	// Đặt góc
	SoftServo_SetAngle(&g_softservo, 0, 0.0f);
}


void mo_cua()
{
	// Đặt góc
	SoftServo_SetAngle(&g_softservo, 0, 90.0f);
}

char check_id()
{
	/*
  if(strcmp(rfid_code,rfid_code1)==0 || strcmp(rfid_code,rfid_code2)==0 || strcmp(rfid_code,rfid_code3)==0)
  {
	  memset(rfid_code,0,sizeof(rfid_code));
	  rfid_valid=THE_DUNG;
	  return 1;
  }
  else if(strlen(rfid_code)>0)
  {
	  memset(rfid_code,0,sizeof(rfid_code));
	  rfid_valid=THE_SAI;
  }
  else rfid_valid=RFID_IDE;
  */

 return 0;
}

void nhap_pass()
{
	_clear;
	nhap_phim();
}

void xoa_tam()
{
  for(int i=0;i<20;i++)
  tam[i]=0;
}

//ham nhap va luu gia tri phim vao mang tam
void nhap_phim()
{
char bo_nho;
char dem=0; //dung de dem so ki tu khop voi pass
char kt =0;//dung de dem so lan nhap ssai pass
int d=1;
kp =0;



//*********check the RFID**********

if(rfid_valid==THE_DUNG)
{
  rfid_valid=RFID_IDE;
  goto menu;
}
//*********************************


xoa_tam();

//===================================================================================
while(kt<3)
{
  //lcd.cursor();
  _clear;
  lcd_str(1,1,"password:");

  d=0; //luu ki tu dau tien cua pass vao vi tri 1 cua mang tam
  while(1)
  {
    //hien thi so ki tu da nhap
    if(d>=11) lcd_chr(1,15,(d-1)/10+0x30); else lcd_chr(1,15,' ');
    lcd_chr(1,16,(d-1)%10+0x30);


    if(d<16) set_key();//doc ma phim bam
    //neu viet du 16 ki tu
    else while(1) {set_key(); if(kp==enter || kp==backspace) break; delay(2);}
    if(kp==enter) break;
    if(kp==backspace && tam[d-1]!=0)
    {
      if(d>0) d--;
      tam[d]=0;
      lcd_chr(2,d,' ');

    }
    else if(kp!=backspace && kp!=0)
    {
      tam[d]=kp;
      lcd_chr(2,d,'*');
      //lcd_chr(2,d,kp);
      d++;
    }

  }


  delay_ms(200);
  _clear;
  lcd_str(1,1,"DANG KIEM TRA....");
  delay_ms(300);

  //sim_print("\ntam:"); sim_print(tam); sim_print("\n");
  //sim_print("\nPASS_DEFAULT:"); sim_print(PASS_DEFAULT); sim_print("\n");
  //sim_print("\n\r");


  //--------------TRUONG HOP NHAP SAI PASS----------------------------------------
  if(strcmp(tam,PASS_DEFAULT)!=0)
  {
    _clear;
    lcd_str(1,1,"NHAP SAI PASS");
    lcd_str(2,1,"LAN THU:");
    lcd_chr(2,11,kt+1+0x30);//kt ban dau khoi tao la 0
    delay_ms(1000);
    _clear;

    if(kt+1<3)// neu nhap pass chua qua 3 lan
    {
      lcd_str(1,1,"PASS KHONG SAI");
      lcd_str(2,1,"QUA 3 LAN");
    }

    if(kt+1==3)//neu nhap pass du 3 lan
    {
      lcd_str(1,1,"DA QUA SO LAN");
      lcd_str(2,1,"NHAP CHO PHEP !");

      //EEPROM_Write(9,1);
    }

    delay_ms(1000);
    kt++;
    d=0;
    dem=0;
  }

  //---------------------TRUONG HOP NHAP DUNG PASS-------------------
  else
  {
    //EEPROM_Write(9,1);//danh dau la nhap dung
    delay(5);
    _clear;
    lcd_str(1,4,">>CUA MO<<");
    //mo_cua();
    kt=9;
    break;
  }
}//while(kt<3)






//-----------------------Xu li khi nhap sai pass---------------------
while(kt!=9)
{
  _clear;
  //sim_send_sms(phone_number, "Canh bao khoa ban phim");
  lcd_str(1,1,"Dang gui sms");
  HAL_Delay(1000);
  _clear;
  lcd_str(1,5,"DA KHOA");
  lcd_str(2,4,"BAN PHIM !");
  /*
  HAL_GPIO_WritePin(DP2_GPIO_Port,DP2_Pin, 1);
  HAL_Delay(3000);
  HAL_GPIO_WritePin(DP2_GPIO_Port,DP2_Pin, 0);
  */

  while(1)
  {
	set_key();        //lien tuc kiem tra nut nhan
	if(rfid_valid==THE_DUNG)
	{
	  _clear;
	  lcd_str(1,1,"Mo khoa");
	  lcd_str(2,1,"ban phim");
		kt=9;
		delay(1000);
		_clear;
		break;
	}
	else if(rfid_valid==THE_SAI)
	{
	  //memset(rfid_code,0,sizeof(rfid_code));
	  _clear;
	  lcd_str(1,1,"The rfid");
	  lcd_str(2,1,"khong dung !");
	  delay(1500);
	  _clear;
	  lcd_str(1,5,"DA KHOA");
	  lcd_str(2,4,"BAN PHIM !");
	}
  }
}


//-----------------------Xu li khi nhap dung pass---------------------
while(kt==9)
{
  menu:
  _clear;
  lcd_str(1,1,"Dang mo khoa !");
  mo_cua();
  _clear;
	lcd_str(1,1,"1-CLOSE");
	lcd_str(2,1,"2-CHANGE PASS");
  while(1)
  {
    set_key();

    //*********check the RFID**********
    if(rfid_valid==THE_DUNG)
    {
    	rfid_valid=RFID_IDE;
      _clear;
      kp=49;
    }
    //*********************************

    if(kp=='1') { kp=0; kt=0; _clear; lcd_str(1,1,"Dang dong cua"); dong_cua(); break; }      //49 la ma ascii cua phim so 1
    if(kp=='2') doi_pass();  //50 la ma ascii cua phim so 1
    /*
    if(kp=='3')
    {
    	_clear;
        lcd_str(1,1,"4-BACK 5-SIM");
        lcd_str(2,1,"6-FINGER");
    }
    if(kp=='4')
    {
	  _clear;
		lcd_str(1,1,"1-CLOSE 3-NEXT");
		lcd_str(2,1,"2-CHANGE PASS");
    }
    if(kp=='5')
	{
		sim_manager();
    	_clear;
        lcd_str(1,1,"4-BACK 5-SIM");
        lcd_str(2,1,"6-FINGER");
	}
    */


  }// while(1)


}//while(kt==9)

}


void old_pass()
{
	char tam[6];
	char dem=0;
	_clear;
	lcd_str(1,1,"old password:");
	nhap_phim_cu();
}



void nhap_phim_cu()
{
char bo_nho;
char dem=0;
char kt =0;
int d=1;
kp =0;


xoa_tam();

//===================================================================================
while(kt<3)
{
  //lcd.cursor();
  _clear;
  lcd_str(1,1,"old password");
  d=0; //luu ki tu dau tien cua pass vao vi tri 1 cua mang tam
  while(1)
  {
    if(d>=11) lcd_chr(1,15,(d-1)/10+0x30); else lcd_chr(1,15,' ');
    lcd_chr(1,16,(d-1)%10+0x30);

    if(d<16) set_key();//doc ma phim bam
    else while(1) {set_key(); if(kp==enter || kp==backspace) break; delay(2);}
    if(kp==enter) break;
    if(kp==backspace && tam[d-1]!=0)
    {
      if(d>1) d--;
      tam[d]=0;
      lcd_chr(2,d,' ');

    }
    else if(kp!=backspace)
    {
      tam[d]=kp;
      lcd_chr(2,d,'*');
      d++;
    }
  }
//===================================================================================






  delay_ms(200);
  _clear;
  lcd_str(1,1,"DANG KIEM TRA...");
  delay_ms(300);


  //------------------------------xu li khi nhap sai pass---------------------
  if(strcmp(tam,PASS_DEFAULT)!=0)
  {
    _clear;
    lcd_str(1,1,"NHAP SAI PASS");
    lcd_str(2,1,"LAN THU:");
    lcd_chr(2,11,kt+1+0x30);
    delay_ms(1000);
    _clear;
    if(kt+1!=3)
    {
      lcd_str(1,1,"PASS KHONG SAI");
      lcd_str(2,1,"QUA 3 LAN");
    }
    if(kt+1==3)
    {
      lcd_str(1,1,"DA QUA SO LAN");
      lcd_str(2,1,"NHAP CHO PHEP !");

    }
    delay_ms(1000);
    kt++;
    d=0;
    dem=0;
  }

  //----------------------xu li khi nhap dung pass---------------------------
  else
  {

    //EEPROM_Write(9,1);//khi nhap dung
    _clear;
    // for(int i=1;i<16;i++) {EEPROM_Write(i,0); delay(5);}
    lcd_str(1,1,"NEW PASSWORD:");

    d=0;
    while(1)
    {
      if(d>=11) lcd_chr(1,15,(d-1)/10+0x30); else lcd_chr(1,15,' ');
      lcd_chr(1,16,(d-1)%10+0x30);

      if(d<16) set_key();//doc ma phim bam
      else while(1) {set_key(); if(kp==enter || kp==backspace) break; delay(2);}
      if(kp==enter) break;
      if(kp==backspace && tam[d-1]!=0)
      {
        if(d>0) d--;
        //EEPROM_Write(d,0); delay(5);
        PASS_DEFAULT[d]=0;
        lcd_chr(2,d,' ');

      }
      else if(kp!=backspace)
      {
        tam[d]=kp;
        //EEPROM_Write(d,kp); delay(5);
        PASS_DEFAULT[d]=kp;
        lcd_chr(2,d,kp);
        d++;
      }
    }
      delay_ms(200);
      kt=9;

    }

}   //while(kt<3)



while(kt!=9)
{
  _clear;
  lcd_str(1,5,"DA KHOA");
  lcd_str(2,4,"BAN PHIM !");
  //_0(coi);
  //EEPROM_Write(10,2);//----------------------khi sai qua 3 lan dua ve pass mac dinh
  while(1);
}


while(kt==9)
{

  _clear;
  lcd_str(1,1,"DANG CAP NHAT");
  lcd_str(2,1,"PASSWORD MOI..");
  delay_ms(1500);
  _clear;
  lcd_str(1,1,"MAT KHAU MOI LA:");
  lcd_str(2,1,PASS_DEFAULT);
  delay_ms(2000);
  _clear;
  break;
}


}//void


void sim_manager()
{
	_clear;
    while(1)
    {
    	lcd_string(1, 1, "1-PHONE 3-EXIT");
        lcd_string(2, 1, "2-EDIT PHONE");
    	set_key();
    	if(kp=='1')
    	{
    		_clear;
    		lcd_string(1, 1, "Phone registered");
    		lcd_string(2, 1, phone_number);
    		HAL_Delay(3000);
    		_clear;
    	}
    	if(kp=='3') break;
    	if(kp=='2')
    	{
    		_clear;
    		 lcd_string(1, 1, "Input Phone");
    		  char d=0; //luu ki tu dau tien cua pass vao vi tri 1 cua mang tam
    		  while(1)
    		  {
    		    if(d<16) set_key();//doc ma phim bam
    		    //neu viet du 16 ki tu
    		    else while(1) {set_key(); if(kp==enter || kp==backspace) break; delay(2);}
    		    if(kp==enter)
				{
    		    	memset(phone_number,0,sizeof(phone_number));
    		    	memcpy(phone_number,tam,strlen(tam));
    		    	_clear;
    		    	lcd_string(1, 1, "saved...");
    		    	HAL_Delay(1000);
					break;
				}
    		    if(kp==backspace && tam[d-1]!=0)
    		    {
    		      if(d>0) d--;
    		      tam[d]=0;
    		      lcd_chr(2,d,' ');

    		    }
    		    else if(kp!=backspace && kp!=0 && kp!='C' && kp!='D')
    		    {
    		      tam[d]=kp;
    		      lcd_chr(2,d,kp);
    		      d++;
    		    }
    		  }
    	    	lcd_string(1, 1, "1-PHONE 3-EXIT");
    	        lcd_string(2, 1, "2-CHANGE");
    	}//if(kp=='2')


    }//while(1)
}








































// TCMATH
// Telescope Controller 4.0 - Math functions
#include "tcheader.h"

/*
( EQC MAKAZDIR GETCURRENTAZAL GOTOAZAL )
( FINDGST FINDLONG FINDLST<-LONG FINDH FINDLST )
( FINDLARGET FINDSMPLARGET )
( STACKSMPFVALS SERPARAMS SERTRIGARG SMPSERIES )
( LPARAX SUNPWD FINDSMPOS SMPCALC GETSMPOSITION )
 */

double RAAZtoF2PIRAD(long d) {
  // frad = d * 2PI / seconds in 360 degrees
  return F2PIdiv360 * (double)(d);
}

double DECALtoFPIdiv2RAD( long d ) {
  // frad = d * 2PI / seconds in 360 degrees
  return RAAZtoF2PIRAD(d);
}

long F2PIRADtoRAAZ( double f ) {
  // d = frad * secs_in_360_deg / 2PI
  return (long)(F360div2PI * f);
}

long FPIdiv2RADtoDECAL( double f ) {
  // d = frad * 2^15-1 / PI/2
  return F2PIRADtoRAAZ(f);
}

void T2toT3( long ra, long decal, long* Dra, long* Ddec ) {
// Convert now: ra/dec - Dra/Ddec
double fd;
// do declination/altitude first
fd = FPIdiv2 * (double)(decal) / F2to15minus1;
*Ddec = FPIdiv2RADtoDECAL(fd);
// d = n * PI/2 2>15-1
// Do RA now
fd = F2PI * (double)(0xffffL && ra) / F2to16;
*Dra = F2PIRADtoRAAZ(fd);
// d = u * 2PI / 2>16
}

void T3toT2( long Dra, long Ddec, long* ra, long* decal ) {
// Convert: Dra/Ddec - ra/dec
double ftmp;
ftmp = DECALtoFPIdiv2RAD(Ddec) * F2to15minus1;
ftmp = ftmp / FPIdiv2;
*decal = FtoS(ftmp);  // - Dra/dec)

ftmp = RAAZtoF2PIRAD(Dra) * F2to16;
ftmp = ftmp / F2PI;
*ra = FtoD(ftmp) & 0xffff;
}

void atUCOORD( int coordnum, long* Dra, long* Ddec ) {
  //int offset;
  long ra, decal;
  
  //offset = 4 * coordnum + 36; // offset to RTC RAM
  //ra = atCLK( offset );
  ra = eecharbuf.strunion.UCOORD[coordnum];
  //decal = atCLK( offset + 2 );
  decal = eecharbuf.strunion.UCOORD[coordnum + 1];
  T2toT3(ra, decal, Dra, Ddec);
}

void storeUCOORD( long Dra, long Ddec, long coordnum ) {
  long ra, decal;
  //int offset;

  //offset = 4 * coordnum + 36; // offset to RTC RAM
  // Convert now
  T3toT2( Dra, Ddec, &ra, &decal );
  //storeCLK( ra, offset );
  eecharbuf.strunion.UCOORD[coordnum] = ra;
  //storeCLK(decal, offset + 2);
  eecharbuf.strunion.UCOORD[coordnum + 1] = decal;
}

void atSMPCOORD( int coordnum, long* Dra, long* Ddec ) {
  *Dra = SMPARRra[coordnum];
  *Ddec = SMPARRdec[coordnum];
}

void storeSMPCOORD( long Dra, long Ddec, int coordnum ) {
  SMPARRra[coordnum] = Dra;
  SMPARRdec[coordnum] = Ddec;
}

double FALplusRG( double f1, double f2 ) {
  double ftmp;
  ftmp = f1 + f2;
  if (ftmp > FPIdiv2) {
    ftmp = FPIdiv2;
  }
  return ftmp;
}

double FALminusRG( double f1, double f2 ) {
  double ftmp;
  ftmp = f1 - f2;
  if ((ftmp + FPIdiv2) < 0.0) {
    ftmp = FminusPIdiv2;
  }
  return ftmp;
}

double INRNG( double f ) {
  // Make sure f is between 0 and 2PI
  while (f > F2PI) { f = f - F2PI; }
  while (f < F0) { f += F2PI; }
  return f;
}

double SINRNG( double f ) { // f seconds
  // Make sure f is between 0 and seconds-in-24-hrs
  while (f < F0) { f += F86400; }
  while (f > F86400) { f = f - F86400; }
  return f;
}

double FdotUT() { // return # of days
  double tmp;
  tmp = GETIME( &UT ) / F86400; // get d seconds / secsperday
  return tmp;
}

long MJD() { // return # of days )
  // mjd=367*yr - 7*[yr+<m+9>/12]/4 + 275*m/9 + day - 678987.
  long mjd;
  mjd = (367L * UTYEAR()) - (7L * (UTYEAR() + ((UTMONTH() + 9)/12L)) / 4L)
    + ((275L * UTMONTH()) / 9L) + UTDAY() - 678987L;
  return mjd;
}

double FMJD() {
  double tmp;
  tmp = (double)MJD() + FdotUT();
  return tmp;
}

void POFFSET( long Dra, long Ddec, double fmjd, double* fradif, double* fdecdif ) {
  FDEC = DECALtoFPIdiv2RAD(Ddec);
  FRA = RAAZtoF2PIRAD(Dra);
  *fradif = (51544.0 - fmjd) * 
        (sin(FRA) * tan(FDEC)* 2.668E-7 + 6.112e-7);
  *fdecdif = (51544.0 - fmjd) * cos(FRA) * 2.655E-7;
}

void PRECESS( long Dra, long Ddec, long* Dpra, long* Dpdec ) {
// for epoch 2000 to current JD
double fradif, fdecdif;
POFFSET(Dra, Ddec, FMJD(), &fradif, &fdecdif);
FDEC = FALminusRG( FDEC, fdecdif);
*Dpra = F2PIRADtoRAAZ(INRNG(FRA - fradif));
*Dpdec = FPIdiv2RADtoDECAL(FDEC);
}

void PRECESSTO2000( long Dra, long Ddec, int epoch, long* Dpra, long* Dpdec ) {
double fmjd, fradif, fdecdif;
long mjd;
mjd = ((long)epoch * 367L) - ((7L * (long)epoch) / 4L) - 678987L;
fmjd = (double) mjd;
POFFSET( Dra, Ddec, fmjd, &fradif, &fdecdif );
FDEC = FALplusRG(fdecdif, FDEC);
*Dpra = F2PIRADtoRAAZ(INRNG(fradif + FRA));
*Dpdec = FPIdiv2RADtoDECAL(FDEC);
}

void PRECESSION( long Dra, long Ddec, int epoch, long* Dpra, long* Dpdec ) { // general routine
  if (epoch != 2000) PRECESSTO2000( Dra, Ddec, epoch, Dpra, Dpdec );
  PRECESS( Dra, Ddec, Dpra, Dpdec );
}

void PRECESSIONTAB( long* Dra, long* Ddec ) {
  long pDpra, pDdec;
  pDpra = *Dra;
  pDdec = *Ddec;
  if (PRECESSquestion()) PRECESS( pDpra, pDdec, Dra, Ddec );
}

void ROFFSET( long Dal, double* fal, double* faldif ) {
*fal = DECALtoFPIdiv2RAD(Dal);
if (*fal < 0.087266463) { //Altitude angle less than 15 degrees in radians?
  double al = (90.0 * *fal) / FPIdiv2; // convert to degrees first
  //faldif=REFRACTCO*17*871.94412[exp[-0.5352051*al]+exp[-0.107041*al]]
  *faldif = FREFRACTCO * 14823.05 * (exp(-0.5352051 * al) + exp(-0.107041 * al));
  // FREFRACTCO=P/[460+T]
} else {
  double za = FPIdiv2 - *fal; // convert to zenith angle
  double ftanza = tan(za);
  //*faldif = (57.626039 * tan(za) - 5.813517E-2 * tan(za)^3) * 17.0 * FREFRACTCO;
  *faldif = (979.642664 * ftanza - 0.98829789 * pow(ftanza, 3.0)) * FREFRACTCO;
  // FREFRACTCO=P/[460+T]
}
//convert arcsec's to radians
*faldif = *faldif * FPIdiv2 / 324000. ;
}

long ANTIREFRACT( long Dral ) {
  // return true altitude
  double fal;
  double faldif;
  double ftmp;
  long Dal = Dral;
  if (REFRACTquestion) {
    ROFFSET(Dral, &fal, &faldif);
    ftmp = FALminusRG(fal, faldif);
    Dal = FPIdiv2RADtoDECAL(ftmp);
  }
  return Dal;
}

void REFRACTION( long Dal, long*  Dral, boolean *alflag ) {
  // return apparent altitude
  // First, only do this if true altitude is > 0. arcsec
  double fal;
  double faldif;
  double ftmp;
  if ((Dal >= 0L) and REFRACTquestion()) {
    ROFFSET(Dal, &fal, &faldif);
    fal = FALplusRG( fal, faldif );
    *alflag = (fal < 0.); // generate alflag )
    *Dral = FPIdiv2RADtoDECAL(fal);
  } else {
    fal = DECALtoFPIdiv2RAD( Dal );
    *alflag = (fal < 0.); // generate alflag )
    *Dral = Dal;
  }
}

void ERROROFF( double faz, double fal, double* rfaz, double* paz, double* rfal, double* pal ) {
  FALTEMP = fal; //save copy of altitude
  double fsinfal = sin(fal);
  *paz = (FCEW / cos(fal)) + (fsinfal * FRHO);
  *rfaz = faz;
  *pal = fsinfal * pow(FRHO, 2.0 );
  *rfal = fal;
}

void ERRORADJ( double faz, double fal, double* fnaz, double* fnal ) {
//           ( fha/fdec - fha'/fdec' )
  *fnaz = faz;
  *fnal = fal;
  if (USERRquestion()) {
    double rfaz;
    double paz;
    double rfal;
    double pal;
    ERROROFF( faz, fal, &rfaz, &paz, &rfal, &pal );
    *fnal = FALplusRG(rfal, pal);
    *fnaz = INRNG(rfaz + paz);
  }
}

void ANTIERRORADJ( double fnaz, double fnal, double* faz, double* fal ) {
//               ( fha/fdec - fha'/fdec' )
  *faz = fnaz;
  *fal = fnal;
  if (USERRquestion()) {
    double rfaz;
    double paz;
    double rfal;
    double pal;
    ERROROFF( fnaz, fnal, &rfaz, &paz, &rfal, &pal );
    *fal = FALminusRG(rfal, pal);
    *faz = INRNG(rfaz - paz);
  }
}

void TRKSETUP() {
// Setup to do fast tracking conversion
double fdec;
FTDEC = DECALtoFPIdiv2RAD( TDEC );
FTSDEC = sin(FTDEC);
FSLstarSD = FSLAT * FTSDEC; //=sin<lat>*sin<dec>
FCLstarCD = FCLAT * cos(FTDEC); //=cos<lat>*cos<dec>
}

void TRKCON( double fha, double* faz, double* fdal ) {
// ( fha - faz/fdal )
// Fast tracking conversion
double ftmp, ftmp2, fx;
FRA = fha;
ftmp = FSLstarSD + (FCLstarCD * cos(fha));
// Make sure value is in range )
if (ftmp > 1.0) {
  ftmp2 = FPIdiv2;
  ftmp = 1.0;
} else {
  ftmp2 = asin(ftmp);
}
*fdal = ftmp2;
// ftmp2 = altitude, ftmp = sin<al>
// fx=[sin<dec>-sin<lat>*sin<al>]/[cos<lat>*cos<al>]
fx = (FTSDEC - (FSLAT * ftmp)) / (cos(ftmp2)* FCLAT);
// check and make sure in range for ACS
if (abs(fx) > 1.0) {
  if (fx < 0.0)
    fx = FPI;
  else
    fx = 0.;
} else {
  fx = acos(fx);
}
if (sin(FRA) >= 0.0) { //is it not negative?
  fx = F2PI - fx;
}
*faz = fx;
}

void ACON( double fazha, double faldec, double *fhaaz, double *fdecal ) {
double ftmp, fx;
FDEC = faldec;
FRA = faldec;
//( fal=asn[sin<lat>*sin<dec>+cos<lat>*cos<dec>*cos<lha>] )
ftmp = (cos(FRA) * cos(FDEC) * FCLAT) + (sin(FDEC) * FSLAT);
//( Make sure value is in range )
if (ftmp > 1.0)
  ftmp = FPIdiv2;
else
  ftmp = asin(ftmp);
//( - altitude or declination )
*fdecal = ftmp;
//( fx=[sin<dec>-sin<lat>*sin<al>]/[cos<lat>*cos<al>] )
fx = (sin(FDEC) - (FSLAT * sin(*fdecal))) / (cos(*fdecal) * FCLAT);
//( check and make sure in range for ACS )
if (abs(fx) > 1.0) {
  if (fx < 0.0)
    fx = FPI;
  else
    fx = 0.;
} else {
  fx = acos(fx);
}
if (sin(FRA) >= 0.0) { //is it not negative?
  fx = F2PI - fx;
}
*fhaaz = fx;
}

void ECON(double fazha, double faldec, double *fhaaz, double *fdecal) {
double ftmp, fx;
//( fazha/faldec - fazha/faldec/fhaaz/fdecal )
FDEC = faldec;
FRA = fazha;
//( fal=asn[sin<lat>*sin<dec>+cos<lat>*cos<dec>*cos<lha>] )
ftmp = (cos(FRA) * cos(FDEC) * FECLAT) + (sin(FDEC) * FESLAT);
//( Make sure value is in range )
if (ftmp > 1.0)
  ftmp = FPIdiv2;
else
  ftmp = asin(ftmp);
//( - altitude or declination )
*fdecal = ftmp;
//( fx=[sin<dec>-sin<lat>*sin<al>]/[cos<lat>*cos<al>] )
fx = (sin(FDEC) - (FESLAT * sin(*fdecal))) / (cos(*fdecal) * FECLAT);
//( check and make sure in range for ACS )
if (abs(fx) > 1.0) {
  if (fx < 0.0)
    fx = FPI;
  else
    fx = 0.;
} else {
  fx = acos(fx);
}
if (sin(FRA) >= 0.0) { //is it not negative?
  fx = F2PI - fx;
}
*fhaaz = fx;
}

double FHARADminus( double frad ) {
  // return fhrangle
  long tmp = GETIME( &LST );
  return (double)tmp * F2PIdiv24HR - frad;
}

double RAtoFHARAD( long Dra ) {
// Return Hour Angle as double
//( lha=lst-ra )
double frad = RAAZtoF2PIRAD( Dra );
double lha = FHARADminus( frad );
return lha;
}

long FHARADtoRA( double fharad ) {
// Return long RA
//( ra=lst-ha )
double lha = FHARADminus( fharad );
double tmp = INRNG( lha );
F2PIRADtoRAAZ( tmp );
}

void RAdivDECtoAZdivAL( long Dra, long Ddec, long* Daz, long* Dal ) {
double fha = RAtoFHARAD( Dra );
double fdec = DECALtoFPIdiv2RAD( Ddec );
double faz, fal;
ACON( fha, fdec, &faz, &fal );
*Dal = FPIdiv2RADtoDECAL( fal );
*Daz = F2PIRADtoRAAZ( faz );
}

void TRACKtoAZdivAL( long* Daz, long* Dal ) {
long Dra = TRA;
double fha = RAtoFHARAD( Dra );
double faz, fdal;
TRKCON( fha, &faz, &fdal );
*Dal = FPIdiv2RADtoDECAL( fdal );
*Daz = F2PIRADtoRAAZ( faz );
}

void AZdivALtoRAdivDEC( long Daz, long Dal, long* Dra, long* Ddec ) {
  double fharad, fdec, faz, fal;
faz = RAAZtoF2PIRAD(Daz);
fal = DECALtoFPIdiv2RAD(Dal);
ACON( faz, fal, &fharad, &fdec );
*Ddec = FPIdiv2RADtoDECAL(fdec);
*Dra = FHARADtoRA(fharad);
}

void EQC ( double* fha, double* fdec ) {
// Do corrections for German Equatorial Mount
// less than 180 deg?
if ( *fdec < FPI ) {
  *fdec = FPIdiv2 - *fdec; // convert to encoder format
} else {
  *fdec = (FPIdiv2 * 3.) - *fdec; // fdec'=f3PI/2-fdec
  if (*fha < FPI) {
    *fha += FPI;
  } else { 
    *fha -= FPI;
  } // modify fha also
}
}

int MAKAZDIR( long Dtazha, long Dazha ) {
  int azhadir;
// target less than current?
if ( Dazha > Dtazha ) {
  if ((Dazha - Dtazha) < 648000L) { //less than 180 deg?
    //( go normal )
    azhadir = -1; // ccw
  } else { // go opposite of normal
    azhadir = 0; // cw
  } //( - azdir )
} else {
  if ((Dtazha - Dazha) < 648000L) { //go normal
    azhadir = 0; // cw
  } else { // opposite
    azhadir = -1; // ccw
  }
}
return azhadir;
}
/*
void GETCURRENTAZAL(long* Daz, long* Dal) {
RDAZAL(); // Results in RAAZ and DECAL
// convert to standard format
if (EQquestion()) {
  ( convert to azal format: TEQ->AZ/AL )
  >2R ( - Dtha ) ( save dec )
  D->F F2PI F*
  MRAAZ L@ D->F F/ ( - fha )
  2R> ( - fha/Dtdec )
  D->F F2PI F*
  MDECAL L@ D->F F/ ( - fha/fdec )
  EQC ( correct for German mount )
  ANTIERRORADJ ( fha/fdec - fha'/fdec' )
  ECON ( - fha/fdec/faz/fal )
  ( FDUP F0< >R ) ( generate alflag )
  FPI/2RAD->DECAL >2R
  ( subtracting AZOFF from the current AZ gives True AZ. )
  F2PIRAD->RAAZ AZOFF L@ D-
  2DUP D0< IF 1296000. D+ THEN
  >2R FDROP FDROP 2R> 2R>
} else { ( alt-az get current az/al )
  ( - Dpaz/Dpal )
  >2R ( - paz )
  D->F F2PI F*
  MRAAZ L@ D->F F/ ( - faz )
  2R> D->F FPI/2 F*
  MDECAL L@ D->F F/ ( - faz/fal )
  ANTIERRORADJ ( faz/fal - faz'/fal' )
  FPI/2RAD->DECAL ( - faz/Dal )
  >2R
  F2PIRAD->RAAZ ( - Daz )
  2R> ( - Daz/Dal )
THEN
}

/*
: GOTOAZAL ( Dtaz/Dtal - )
EQ? IF
  ( convert to eq format here: AZ/AL->TEQ )
  ( Add AZOFF to True AZ to get mount AZ. )
  >2R AZOFF L@ D+ RAAZ->F2PIRAD INRNG 2R>
  DECAL->FPI/2RAD ( - faz/fal )
  ECON ( - faz/fal/fha/fdec ) ( answers in radians! )
  ERRORADJ
  ( convert to encoder format )
  FPI/2 FSWAP F- ( - faz/fal/fha/fedec )
  FORK? 0= ( German Equatorial Mount? )
  IF
    ( Check to see if we need to flip the axis around )
    ( to view stuff coming up in the East. )
    FOVER FPI F> ( is fha > PI? )
    IF
      FPI F+ ( go 180 deg off in dec ) ( - fdec' )
      FSWAP ( - fdec'/fha )
      ( Already know that fha > PI, so subtract PI )
      ( to flip it. )
      FPI F- FSWAP ( - fha'/fdec' )
    THEN
  THEN ( - faz/fal/fha/fedec )
  ( compute ha direction )
  FOVER ( - faz/fal/fha/fdec/fha )
  F2PIRAD->RAAZ
  RDAZAL ( - faz/fal/fha/fdec/Dha/Dxha/Dydec )
  >2R D->F ( correct ha first )
  ( ha = xha * 1296000 / max )
  F1296000. ( - faz/fal/fha/fdec/Dha/fxha/F1296000 )
  F* ( - faz/fal/fha/fdec/Dha/f )
  MRAAZ L@ D->F F/ ( - faz/falfha/fdec/Dha/fxha )
  F->D 2R> ( - Dha/Dxha/Dydec )
  MAKAZDIR ( - faz/fal/fha/fdec/hadir )
  >R ( save temp )
  MDECAL L@ D->F F* ( - faz/fal/fha/fdec'' )
  F2PI F/ F->D ( - faz/fal/fha/Dtdec )
  2DUP TAL L! ( store target )
  >2R ( - faz/fal/fha )
  MRAAZ L@ D->F F*
  F2PI F/ F->D ( - faz/fal/Dtha )
  2DUP TAZ L! ( - faz/fal/Dtha )
  >2R ( save Dtha )
  FDROP FDROP 2R> 2R> ( - Dtha/Dtdec )
  R> 0 2ROT 2ROT ( - Dhadir/Dtha/Dtdec )
ELSE ( alt-az goto az/al )
  ( - Dtaz/Dtal )
  2OVER GETCURRENTAZAL ( - Dtaz/Dtal/Dtaz/Daz/Dal )
  MAKAZDIR ( - Dtaz/Dtal/azdir )
  0 2ROT 2ROT ( - Dazdir/Dtaz/Dtal )
  >2R ( - Dtaz )
  RAAZ->F2PIRAD ( - Dazdir/faz )
  2R> DECAL->FPI/2RAD ( - Dazdir/faz/fal )
  ERRORADJ ( - faz/fal )
  ( Dpal = Dal * Dmdecal / secs in 90deg )
  MDECAL L@ D->F F*
  FPI/2 F/
  F->D ( - Dpal )
  TAL L! ( store target )
  ( - faz )
  MRAAZ L@ D->F F*
  F2PI F/
  F->D ( - Dpaz )
  2DUP TAZ L!
  TAL L@ ( - Dpaz/Dpal )
THEN
5 ROLL DROP ( Gets rid of the zero in front of azdir )
MTRSON ( azdir/traaz/tdecal - ) ;

DECIMAL
: FINDGST ( - fgst in seconds of time at gmt base time )
( ftu=[mjd-51544.5]/36525 )
MJD D->F F51544.5 F-
F36525. F/ ( - ftu )
( fgst=24110.54841+8640184.812866*ftu+0.093104*ftu^2-6.2e-6*ftu^3 )
FDUP FDUP F* FOVER FOVER F* ( ftu - ftu/ftu^2/ftu^3 )
F-6.2E-6 F* FSWAP ( - ftu/fgst?/ftu^2 )
F0.093104 F* F+ FSWAP ( - fgst?/ftu )
F8640184.812866 F* F+ ( - fgst? )
F24110.54841 F+ ( - fgst in seconds at 0 hrs gmt )
UT 2@ 2DUP 365 U/ ( get lst of base )
SWAP DROP ( don't need remainder )
S->D D+
D->F F+ ( - fgst )
( now check boundary conditions )
SINRNG ;

: FINDLONG ( - )
( flong=flst-fgst )
LST 2@ ( - d ) ( lst seconds of base )
D->F
FINDGST ( - flst/fgst )
F- ( - flong in seconds )
F2PI/24HR F* ( convert to radians )
( put in range +-180 degrees )
FDUP FPI F> IF F2PI F- THEN
FDUP FPI F-1. F* F< IF F2PI F+ THEN
FLONGITUDE F! ;

: FINDLST<-LONG ( - )
( flst=fgst+flong )
( note: long must be in seconds, not radians! )
FINDGST ( - fgst )
FLONGITUDE F@ ( - fgst/flong )
( convert to seconds of time/day )
F24HR/2PI F* ( - fgst/flongn )
F+ ( - f )
SINRNG ( check for overflow, underflow )
F->D LST 2! ;

: FINDH ( Daz/Dal/Dra/Ddec - fha/Dra )
2SWAP >2R >2R ( dec on top of return stack )
( h'=[sin<al>-sin<lat>*sin<dec>]/[cos<lat>*cos<dec>] )
DECAL->FPI/2RAD FSIN
FSLAT F@ 
2R DECAL->FPI/2RAD FSIN F* F-
FCLAT F@ 
2R> DECAL->FPI/2RAD FCOS F* F/ ( - az/fh' )
FACS SERFNUM F! ( temp store of HA )
RAAZ->F2PIRAD ( az - faz )
FSIN F0. F> ( check HA boundary condition )
IF
  F2PI SERFNUM F@ F-
ELSE
  SERFNUM F@
THEN
2R> ; ( - fha/Dra ) ( get ra off return stack )

: FINDLST ( Daz/Dal/Dra/Ddec - )
( time of star coords taken = # seconds from base time stored in STARTIME )
FINDH ( - fha/Dra )
( lst=arcos<h'>+ra )
RAAZ->F2PIRAD F+ ( LST in radians )
F24HR/2PI F* ( LST in seconds )
( check boundary conditions )
SINRNG ( check for overflow, underflow )
F->D STARTIME 2@ 2DUP ( - dlst/rdtime/rdtime )
365 U/ >R ( save seconds correction to LST )
DROP ( don't need remainder )
R> 0 D- ( subtract correction )
D- ( - [dlst-dclocktime] )
( check boundary conditions again )
D->F SINRNG ( check for overflow, underflow )
F->D
LST 2! ( save it ) ;

( Start of SMP routines )
: FINDLARGET ( - )
FMJD F51544.5 F- ( - t )
FDUP FSMALLT F! F36525. F/ FLARGET F! ;

: FINDSMPLARGET FINDLARGET FLARGET F@ F1. F+ FLARGET F! ;

: STACKSMPFVALS ( - fval1/.../fval30/spaddr )
31 0
DO
  ( arg=f1+f2*t )
  I 10 * ( 2f#'s=10 bytes )
  SMPTAB DUP ( - table address/ta )
  @ ( - start of equation table parameters )
  + ( add offset )
  + ( add # bytes up param f#'s )
  DUP >R ( save copy of addr )
  F@ ( get one )
  R> 5 + F@ ( get next )
  FSMALLT F@ F* F+
  FDUP FFIX F- ( keep fractional part only )
LOOP
SP@ DUP SERSP ! ; ( get stack position here + store )

: SERPARAMS
0 ( - #arg#'s/0 )
DO
  SERADDR @ C@ ( - # of param )
  30 SWAP - 6 *
  SERSP @ ( - #/get pointer )
  + F@ ( - get fparam )
  1 SERADDR +! ( update pointer )
LOOP
SP@ SERISP ! ; ( save pointer to top of params used )

: SERTRIGARG ( f - f )
SERANUM C@ 0 ( - f0/#arg#'s/0 )
DO
  SERADDR @ @ ( header bytes )
  2 I ** AND ( mask param bit )
  IF
    SERANUM C@ 1- 6 * ( total bytes to top )
    SERISP @ +
    I 6 * - ( point to param to use )
    F@ ( - f/fparam )
    SERTERM C@ 2/ SERADDR @ + 2+ C@ ( get param coeff )
    SERADDR @ @ ( header bytes )
    8192 AND ( byte/nybble bit ) ( 8192=$2000 )
    IF ( nybble )
      SERTERM C@ 2 MOD
      IF 16 / ELSE 15 AND THEN
      DUP 8 AND
      IF 65520 OR THEN ( 65520=$FFF0 )
      1 SERTERM C+!
    ELSE ( byte )
      DUP 128 AND ( 128=$80 )
      IF 65280 OR THEN ( 65280=$FF00 )
      2 SERTERM C+!
    THEN ( n - n )
    S->F F* F+
  THEN
LOOP ( - f# )
F2PI F* ; ( convert to radians first )

: SMPSERIES ( addr1 - addr2/fnum )
DUP SERADDR ! ( save pointer to table )
C@ DUP SERANUM C! ( save # of argument #'s )
1 SERADDR +! ( update pointer )
SERPARAMS
F0. FW F! ( start with nothing )
BEGIN ( - )
  0 SERTERM C!
  SERADDR @ F@ ( get coeff )
  SERFNUM F! ( and store )
  5 SERADDR +! ( update pointer )
  F0. ( start with nothing for this term )
  SERTRIGARG ( f - f )
  SERADDR @ @
  16384 AND ( mask sin/cos bit ) ( =$4000 )
  IF FCOS ELSE FSIN THEN
  SERADDR @ @
  3072 AND DUP ( mask T bits ) ( =$C00 )
  IF
    1024 / S->F ( =$400 )
    FLARGET F@ FSWAP FDUP F1. F>
    IF F** ELSE FDROP THEN
    F*
  ELSE
    DROP
  THEN
  SERFNUM F@ F* ( times coeff )
  FW F@ F+ FW F! ( add to total )
  SERADDR @ @
  4096 AND ( - flag ) ( last term? ) ( =$1000 )
  ( update pointer to next term )
  SERTERM C@ 2 /MOD + ( add remainder to quotient )
  2+ SERADDR +!
UNTIL
SERANUM C@ NFDROP ( clean up stack )
SERADDR @ FW F@ ; ( - addr2/fseries )

: LPARAX ( Dra/Ddec - Dra'/Ddec' )
( Lunar parallax correction )
RA/DEC->AZ/AL ( Dra/Ddec - Dra/Ddec/Daz/Dal )
( get rid of old ra/dec )
4 2ROLL 2DROP
3 2ROLL 2DROP
DECAL->FPI/2RAD FDUP FPI/2 FSWAP F- ( - az/fal/fz )
FDUP FSIN FSWAP FCOS ( -az/fal/fSINz/fCOSz )
FU F@ FSQRT F60.40974 F* ( - az/fal/fSINz/fCOSz/fr )
FSWAP F- F/ FATN ( - az/fal/fparax )
FAL-RG ( - az/fal' )
FPI/2RAD->DECAL
AZ/AL->RA/DEC 2DROP 2DROP ( don't need az/al )
; ( - Dra'/Ddec' )

: SUNPWD ( - flag )
( Prompt for a password when computing solar position.)
( TERMIF ( )
  BCR ABCR
  AB." Enter password to get solar position."
( THEN ( )
CR ." PSD=" ( The password is PI )
KEY 80 =
KEY 73 = AND DUP
IF
  CR ." PASSED" WAITASEC WAITASEC
THEN ;

: FINDSMPOS ( n - Dra/Ddec )
( Do the real calculation here )
>R ( save n on return stack )
FINDSMPLARGET
STACKSMPFVALS ( - fval1/.../fval30/spaddr )
( R=n on return stack )
R 3 =
IF
  0 ( Lm=moon )
ELSE
  R 3 <
  IF
    5 ( use Ls for inner planets )
  ELSE
    R 3 * 1+ ( otherwise use Lp for outer planets )
  THEN
THEN ( - sp/r )
30 SWAP - 6 * + ( compute # of bytes up stack )
F@ F2PI F* FL F!
R ( - n )
2* 2+ ( compute offset to link address )
SMPTAB + ( - addr of link addr )
@ ( - offset to series )
SMPTAB + ( - addr of series )
SMPSERIES ( addr1 - addr2/fnum )
FV F!
SMPSERIES FU F!
SMPSERIES ( FDROP ) ( result in FW already )
DROP ( addr - )
( 31 ) 32 NFDROP ( clean parameters off stack )
( ra=L+asn[W/sqrt<U-V*V>] )
FW F@ FU F@ FV F@ FDUP
F* F- FSQRT F/ FASN
FL F@ F+ INRNG F2PIRAD->RAAZ ( - ra )
( dec=asn[V/sqrt<U>] )
FV F@ FU F@ FSQRT F/ FASN FPI/2RAD->DECAL
R> 3 = NOTINIT? 0= AND
IF ( lunar parallax correction )
  LPARAX
THEN ;

: SMPCALC ( n - Dra/Ddec )
( Handle getting precalculated values to save time )
DUP ABS ( Absolute value of negative # )
15 ( 0F ) AND ( Mask off if tracking cmd )
2 OVER ( - n/n'/2/n' )
** SMPDONE @ AND
IF ( Calc has been done )
  SWAP 9 >
  IF ( Tracking cmds only )
    CR ." RECALC?" GETYORN
    IF
      DUP >R FINDSMPOS ( n - Dra/Ddec )
      2OVER 2OVER R>
      !SMPCOORD ( Dra/Ddec/coord# - )
    ELSE
      @SMPCOORD ( coord# - Dra/Ddec )
    THEN
  ELSE ( For show or init, just get the coords )
    @SMPCOORD ( coord# - Dra/Ddec )
  THEN
ELSE ( Do calc anyway, and store )
  SWAP DROP
  DUP >R FINDSMPOS ( n - Dra/Ddec )
  2 R ** SMPDONE @ OR SMPDONE !
  2OVER 2OVER R>
  !SMPCOORD ( Dra/Ddec/coord# - )
THEN ;

: GETSMPOSITION ( n - Dra/Ddec )
( This routine requires a password for computing solar coords )
DUP ABS ( Absolute value of negative # )
15 ( 0F ) AND ( Mask off if tracking cmd )
0=
IF
  DUP FNDSUN = ( Is this to track the Sun? )
(  OPTIONS @CLK SOLARBIT AND AND ( )
  IF ( yes, so clear the bit )
(    2CR ." FILTER IN" ( )
(    PIA1DRIVE C@ SOLARBIT NOT AND ( )
(    DUP PIA1DRIVE C! ( )
(    PIA1DATA 1+ C! ( )
(    BEGIN ( )
      ( Wait to insert Solar filter )
(      PIA1DATA C@ SOLARBIT AND NOT ( Left on stack when done )
(    UNTIL ( )
(    TRUEFLAG ( )
(  ELSE ( )
    SUNPWD ( - flag )
  THEN
  IF
    ( FINDSMPOS ( n - Dra/Ddec )
    SMPCALC ( n - Dra/Ddec )
  ELSE
    DROP GETCURRENTAZAL ( - Daz/Dal )
    AZ/AL->RA/DEC ( Daz/Dal - Dra/Ddec/Daz/Dal )
    2DROP 2DROP
  THEN
ELSE
  ( FINDSMPOS ( n - Dra/Ddec )
  SMPCALC ( n - Dra/Ddec )
THEN ;

  */

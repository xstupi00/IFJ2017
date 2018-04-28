CREATE OR REPLACE TRIGGER check_identification_number
	BEFORE INSERT OR UPDATE OF ID_NUMBER ON PERSON
  FOR EACH ROW
DECLARE
  ID_NUMBER PERSON.ID_NUMBER%TYPE;
  DAY VARCHAR2(2);
  MONTH VARCHAR2(2);
  YEAR VARCHAR2(2);
  LEN INTEGER;
  FORMAT_ERROR INTEGER;
  DAY_ERROR INTEGER;
  MONTH_ERROR INTEGER;
  YEAR_ERROR INTEGER;
  DIVIDE_ERROR INTEGER;
BEGIN
  ID_NUMBER := :NEW.ID_NUMBER;
  LEN = LENGTH(ID_NUMBER);
  YEAR := SUBSTR(ID_NUMBER, 1, 2);
  MONTH := SUBSTR(ID_NUMBER, 3, 2);
  DAY := SUBSTR(ID_NUMBER, 5, 2);
  
  FORMAT_ERROR := -20100;
  DAY_ERROR := -20200;
  MONTH_ERROR := -20300;
  YEAR_ERROR := -20400;
  DIVIDE_ERROR := -20500;
  
  IF (MONTH > 50) THEN
    MONTH := MONTH - 50;
  ELSE
  IF (MONTH > 20) THEN
          MONTH := MONTH - 20;
       END IF;
   END IF;

   IF (DAY > 40) THEN
    DAY := DAY - 40;
   END IF;

   IF (LEN = 9 OR LEN = 10) THEN 
     IF (LEN != 9 OR SUBSTR(rodne_cislo, 7, 3) != '000') THEN 
       IF (MOD(ID_NUMBER, 11) = 0) THEN
          IF (YEAR >= 0 AND YEAR <= 99) THEN
             IF (MONTH > 0 AND MONTH <= 12) THEN
                IF (DAY > 0) THEN
                  IF ((MONTH = 2 AND MOD(YEAR, 4) = 0 AND (YEAR <= 29)) OR MONTH > 0 AND MONTH <= 12) THEN
                     IF (MONTH = 2 AND MOD(YEAR, 4) != 0 AND (DAY > 28)) THEN
                        IF ((MONTH = 1 OR MONTH = 3 OR MONTH = 5 OR MONTH = 7 OR MONTH = 8 OR MONTH = 10 OR MONTH = 12) AND den > 31) THEN
                          raise_application_error(DAYERROR, 'The given DAY is not in the correct format, you check it!');
                        ELSE  
                           IF ((MONTH = 4 OR MONTH = 6 OR MONTH = 9 OR MONTH = 11) AND den > 30) THEN 
                                raise_application_error(-20003, 'Neplatny datum v rodnom cisle'); END IF;
                        END IF;
                     ELSE 
                       raise_application_error(DAYERROR, 'The given DAY is not in the correct format, you check it!'); END IF
                  ELSE 
                    raise_application_error(DAYERROR, 'The given DAY is not in the correct format, you check it!'); END IF;
                ELSE
                  raise_application_error(DAY_ERROR, 'The given DAY is not in the correct format, you check it!'); END IF;
              ELSE  
                raise_application_error(MONTH_ERROR, 'The given MONTH is not in the correct format, you check it!'); END IF;
          ELSE 
            raise_application_error(YEAR_ERROR, 'The given YEAR is not in the correct format, you check it!'); END IF;
       ELSE 
         raise_application_error(DIVIDE_ERROR, 'The identification number must be divided by number eleven!'); END IF;
     ELSE 
       raise_application_error(FORMAT_ERROR, 'The bad format of the identification number (bad end parts (000)!'); END IF;
   ELSE
     raise_application_error(FORMAT_ERROR, 'The bad format of the identification number (bad count of numbers) !'); END IF;
END;
/

-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	
	-- Program counter register
	signal pc_reg : std_logic_vector(11 downto 0);
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	
	-- Pointer to data memory register
	signal ptr_reg : std_logic_vector(9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	
	-- While cycle register
	signal cnt_reg     : std_logic_vector(11 downto 0);	
	signal cnt_ld_one  : std_logic;
	signal cnt_inc     : std_logic;
	signal cnt_dec     : std_logic;
	
	-- Multiplexor
	signal mux_sel : std_logic_vector(1 downto 0);
	signal mux_val : std_logic_vector(7 downto 0);	
	
	-- Instruction decoder
	type inst_type is (ret, incptr, getchar, decptr, putchar, 
		dec, inc, whilestart, whileend, break, unknown);
	signal inst_dec : inst_type;
	
	-- FSM
	type fsm_state is (sidle, sfetch, sdecode, sret, sincptr, 
							 sgetchar0, sgetchar1, sdecptr, sputchar0, 
							 sputchar1, sdec0, sdec1, sinc0, sinc1, 
							 swhilestart0, swhilestart1, swhilestart2, 
							 swhilestart3, swhileend0, swhileend1, 
							 swhileend2, swhileend3, swhileend4, 
							 sbreak0, sbreak1, sbreak2);	
	signal pstate : fsm_state;
	signal nstate : fsm_state;

begin
	-- Program counter register
	pc: process(CLK, RESET)
	begin
		if (RESET='1') then
			pc_reg <= "000000000000";
		elsif (CLK'event) and (CLK='1') then
			if (pc_inc='1') then
				pc_reg <= pc_reg + 1;
			elsif (pc_dec='1') then
				pc_reg <= pc_reg - 1;			
			end if;
		end if;
	end process;
	
	-- Set CODE_ADDR
	CODE_ADDR <= pc_reg;
	
	-- Pointer to data memory register
	ptr: process(CLK, RESET)
	begin
		if (RESET='1') then
			ptr_reg <= "0000000000";
		elsif (CLK'event) and (CLK='1') then
			if (ptr_inc='1') then
				if (ptr_reg="1111111111") then
					ptr_reg <= "0000000000";
				else
					ptr_reg <= ptr_reg + 1;
				end if;						
			elsif (ptr_dec='1') then
				if (ptr_reg="0000000000") then					
					ptr_reg <= "1111111111";
				else
					ptr_reg <= ptr_reg - 1;
				end if;
			end if;
		end if;
	end process;
	
	-- Set DATA_ADDR
	DATA_ADDR <= ptr_reg;
	
	-- While cycle register
	cnt: process(CLK, RESET)
	begin
		if (RESET='1') then
			cnt_reg <= "000000000000";
		elsif (CLK'event) and (CLK='1') then
			if (cnt_ld_one='1') then
				cnt_reg <= (0 => '1', others => '0');
			elsif (cnt_inc='1') then
				cnt_reg <= cnt_reg + 1;
			elsif (cnt_dec='1') then
				cnt_reg <= cnt_reg - 1;
			end if;		
		end if;
	end process;
	
	-- Value written to RAM multiplexor	
	mux: process(mux_sel, IN_DATA, DATA_RDATA)
	begin
		case mux_sel is
			when "00"   => mux_val <= IN_DATA;
			when "01"   => mux_val <= DATA_RDATA-1;
			when "10"   => mux_val <= DATA_RDATA+1;
			when others => mux_val <= DATA_RDATA;
		end case;		
	end process;
	
	-- Set DATA_WDATA
	DATA_WDATA <= mux_val;
	
	-- Instruction decoder
	process (CODE_DATA)
	begin
		case(CODE_DATA(7 downto 4)) is			
			when X"0" => 
				case(CODE_DATA(3 downto 0)) is
					when X"0"   => inst_dec <= ret;
					when others => inst_dec <= unknown;
				end case;
			when X"2" =>
				case(CODE_DATA(3 downto 0)) is
					when X"B"   => inst_dec <= inc;
					when X"C"   => inst_dec <= getchar;
					when X"D"   => inst_dec <= dec;
					when X"E"   => inst_dec <= putchar;
					when others => inst_dec <= unknown;
				end case;
			when X"3" =>
				case(CODE_DATA(3 downto 0)) is					
					when X"C"   => inst_dec <= decptr;					
					when X"E"   => inst_dec <= incptr;
					when others => inst_dec <= unknown;
				end case;
			when X"5" =>
				case(CODE_DATA(3 downto 0)) is
					when X"B"   => inst_dec <= whilestart;					
					when X"D"   => inst_dec <= whileend;
					when others => inst_dec <= unknown;
				end case;			
			when X"7" => 
				case(CODE_DATA(3 downto 0)) is
					when X"E"   => inst_dec <= break;
					when others => inst_dec <= unknown;
				end case;
			when others => inst_dec <= unknown;
		end case;
	end process;
	
	-- Set I/O OUT_DATA
	OUT_DATA <= DATA_RDATA;
	
	-- FSM register
	fsm_pstate: process (RESET, CLK)
	begin
		if(RESET='1') then
			pstate <= sidle;
		elsif (CLK'event) and (CLK='1') then
			if (EN='1') then
				pstate <= nstate;
			end if;
		end if;
	end process;
	
	-- FSM next state logic
	nstate_logic: process(pstate, inst_dec, cnt_reg, 
								 OUT_BUSY, IN_VLD, DATA_RDATA)
	begin
		pc_inc <= '0'; 
		pc_dec <= '0';
	
		ptr_inc <= '0';
		ptr_dec <= '0'; 
		
		cnt_ld_one <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';

		mux_sel <= "00";
		
		CODE_EN <= '0';
		DATA_EN <= '0';
		DATA_WREN <= '0';		

		IN_REQ <= '0';
		OUT_WREN <= '0';		
		
		case pstate is
			when sidle =>
				nstate <= sfetch;
				
			when sfetch =>
				nstate <= sdecode;
				CODE_EN <= '1';
			
			when sdecode =>
				case inst_dec is
					when ret        => nstate <= sret;
					when incptr     => nstate <= sincptr;
					when getchar    => nstate <= sgetchar0;
					when decptr     => nstate <= sdecptr;					
					when putchar    => nstate <= sputchar0;
					when dec        => nstate <= sdec0;					
					when inc        => nstate <= sinc0;					
					when whilestart => nstate <= swhilestart0;
					when whileend   => nstate <= swhileend0;
					when break      => nstate <= sbreak0;
					when unknown    =>
						nstate <= sfetch;
						pc_inc <= '1';
					when others     => null;
				end case;
				
			when sincptr =>		
				nstate <= sfetch;
				ptr_inc <= '1';
				pc_inc <= '1';			
	
			when sdecptr =>		
				nstate <= sfetch;
				ptr_dec <= '1';
				pc_inc <= '1';	
						
			when sinc0 =>
				nstate <= sinc1;
				DATA_EN <= '1';
				DATA_WREN <= '0';				
				
			when sinc1 =>
				nstate <= sfetch;
				DATA_EN <= '1';
				DATA_WREN <= '1';
				mux_sel <= "10";
				pc_inc <= '1';
			
			when sdec0 =>
				nstate <= sdec1;
				DATA_EN <= '1';
				DATA_WREN <= '0';										
								
			when sdec1 =>
				nstate <= sfetch;										
				DATA_EN <= '1';
				DATA_WREN <= '1';
				mux_sel <= "01";	
				pc_inc <= '1';
						
			when sputchar0 =>
				nstate <= sputchar1;
				DATA_EN <= '1';
				DATA_WREN <= '0';	
								
			when sputchar1 =>
				if (OUT_BUSY='1') then
					nstate <= sputchar1;
					DATA_EN <= '1';
					DATA_WREN <= '0';						
				else
					nstate <= sfetch;
					OUT_WREN <= '1';
					pc_inc <= '1';	
				end if;			
						
			when sgetchar0 =>
				nstate <= sgetchar1;
				IN_REQ <= '1';
				
			when sgetchar1 =>						
				if (IN_VLD='0') then
						nstate <= sgetchar1;
						IN_REQ <= '1';
				else
					nstate <= sfetch;
					DATA_EN <= '1';
					DATA_WREN <= '1';
					mux_sel <= "00";
					pc_inc <= '1';
				end if;
			
			when swhilestart0 =>
				nstate <= swhilestart1;
				pc_inc <= '1';
				DATA_EN <= '1';
				DATA_WREN <= '0';						
				
			when swhilestart1 =>				
				if (DATA_RDATA="00000000") then
					cnt_ld_one <= '1';					
					nstate <= swhilestart2;
				else
					nstate <= sfetch;
				end if;
			
			when swhilestart2 =>
				if (cnt_reg="000000000000") then
					nstate <= sfetch;
				else
					nstate <= swhilestart3;
					CODE_EN <= '1';																					
				end if;
				
			when swhilestart3 =>															
				nstate <= swhilestart2;
				pc_inc <= '1';
					
				if (inst_dec=whilestart) then
					cnt_inc <= '1';
				elsif (inst_dec=whileend) then
					cnt_dec <= '1';					
				end if;								
				
			when swhileend0 =>	
				nstate <= swhileend1;
				DATA_EN <= '1';
				DATA_WREN <= '0';
				
			when swhileend1 =>
				if (DATA_RDATA="00000000") then
					nstate <= sfetch;
					pc_inc <= '1';					
				else
					nstate <= swhileend2;					
					cnt_ld_one <= '1';										
					pc_dec <= '1';
				end if;

			when swhileend2 =>
				if (cnt_reg="000000000000") then
					nstate <= sfetch;
				else
					nstate <= swhileend3;
					CODE_EN <= '1';					
				end if;
			
			when swhileend3 =>								
				nstate <= swhileend4;
					
				if (inst_dec=whileend) then
					cnt_inc <= '1';
				elsif (inst_dec=whilestart) then
					cnt_dec <= '1';					
				end if;																				
				
			when swhileend4 =>
				nstate <= swhileend2;
				
				if (cnt_reg="000000000000") then
					pc_inc <= '1';
				else
					pc_dec <= '1';
				end if;										
							
			when sbreak0 =>
				nstate <= sbreak1;
				cnt_ld_one <= '1';
				pc_inc <= '1';
				
			when sbreak1 =>
				if (cnt_reg="000000000000") then
					nstate <= sfetch;
				else
					nstate <= sbreak2;
					CODE_EN <= '1';
				end if;				
				
			when sbreak2 =>										
				nstate <= sbreak1;
				pc_inc <= '1';
					
				if (inst_dec=whilestart) then
					cnt_inc <= '1';
				elsif (inst_dec=whileend) then
					cnt_dec <= '1';					
				end if;																																		
				
			when sret =>
				nstate <= sret;
	
			when others =>				
				null;
				
		end case;
	end process;	
end behavioral;
 

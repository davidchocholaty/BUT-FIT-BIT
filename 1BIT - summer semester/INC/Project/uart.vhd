-- uart.vhd: UART controller - receiving part
-- Author:  David Chocholaty <xchoch09@stud.fit.vutbr.cz> 
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
    CLK: 	    in std_logic;
	RST: 	    in std_logic;
	DIN: 	    in std_logic;
	DOUT: 	    out std_logic_vector(7 downto 0);
	DOUT_VLD: 	out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
signal clk_cnt   : std_logic_vector(3 downto 0); -- Clock period counter from 0 to 15
signal data_cnt  : std_logic_vector(2 downto 0); -- Count of read data
signal bit_mid   : std_logic;                    -- Middle of bit
signal data_rdy  : std_logic;                    -- All eight bits are read
signal data_read : std_logic;                    -- Reading data
signal data_vld  : std_logic;                    -- Valid data

begin
	FSM: entity work.UART_FSM(behavioral)
	port map (
		CLK => CLK,
		RST => RST,
		DIN => DIN,
		BIT_MID => bit_mid,
		DATA_RDY => data_rdy,		
		DATA_READ => data_read,				
		DATA_VLD => data_vld		
	);
	
	-- Clock period counter from 0 to 15
	-- clk_cnt is increment on rising edge
	-- clk_cnt is reset on RST='1' or on falling edge on DIN
	p_clk_cnt: process(CLK, RST, DIN)
	begin
		if (RST = '1' or falling_edge(DIN)) then							
			clk_cnt <= "0000";		
		elsif rising_edge(CLK) then			
			clk_cnt <= clk_cnt + 1;						
		end if;							
	end process;

	-- Count of read data
	-- Data are ready if all eight bits are read
	p_cmp_data_cnt: process(data_cnt)
	begin
		if data_cnt = "111" then			
			data_rdy <= '1';
		else			
			data_rdy <= '0';
		end if;	
	end process;

	-- Set DOUT bit
	-- On middle of bit in reading process is bit
	-- written on bit position to DOUT and data_cnt is increment
	p_dec_reg: process(CLK, RST)
	begin		
		if RST = '1' then
			DOUT <= "00000000";
			data_cnt <= "000";
		elsif rising_edge(CLK) then			
			if (bit_mid = '1' and data_read = '1') then																			
				DOUT(conv_integer(data_cnt)) <= DIN;				
				data_cnt <= data_cnt + 1;					
			end if;			
		end if;
	end process;
	
	-- Set middle of bit
	-- In the middle of bit is bit_mid set on 1
	-- Otherwise is bit_mid set on 0
	p_cmp_clk_cnt: process(clk_cnt)
	begin
		if clk_cnt = "1000" then			
			bit_mid <= '1';						
		else					
			bit_mid <= '0';						
		end if;
	end process;

	-- Set DOUT_VLD
	-- If readen data are valid is DOUT_VLD set on 1
	-- on one clock period
	p_cmp_data_vld: process(data_vld)
	begin		
		if data_vld = '1' then		
			DOUT_VLD <= '1';			
		else
			DOUT_VLD <= '0';
		end if;
	end process;	
end behavioral;

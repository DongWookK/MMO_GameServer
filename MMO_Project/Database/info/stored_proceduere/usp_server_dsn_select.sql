/* ------------------------------------------------------------
title   : usp_server_dsn_select
desc    : 유저 로그인, 최초생성

2026.09.16 | 김동욱 | 최초 생성
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_server_dsn_select]
	@p_server_no SMALLINT
AS
BEGIN
	SET NOCOUNT ON;
	
	SELECT type, dsn
	FROM [dbo].[tb_server_dsn]
	WHERE server_no = @p_server_no
END
GO
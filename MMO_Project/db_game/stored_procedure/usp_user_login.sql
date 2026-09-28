/* ------------------------------------------------------------
title   : usp_user_login
desc    : 유저 로그인, 최초생성

2026.09.16 | 김동욱 | 최초 생성
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_user_login]
    @p_user_name NVARCHAR(20)
    , @p_user_no INT OUTPUT
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    
    SELECT @p_user_no = user_no 
    FROM [dbo].[tb_user] 
    WHERE user_name = @p_user_name;

    IF @p_user_no IS NULL
    BEGIN
        INSERT INTO [dbo].[tb_user] (user_name, login_time)
        VALUES (@p_user_name, SYSDATETIME());

        SET @p_user_no = SCOPE_IDENTITY();
    END
    ELSE
    BEGIN
        UPDATE [dbo].[tb_user]
        SET login_time = SYSDATETIME()
        WHERE user_no = @p_user_no;
    END
END
GO
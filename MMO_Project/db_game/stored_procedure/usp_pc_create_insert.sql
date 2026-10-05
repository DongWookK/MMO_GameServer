/* ------------------------------------------------------------
title   : usp_pc_create_insert
desc    : 캐릭터 생성

2026.10.05 | 김동욱 | 최초 생성
2026.10.05 | 김동욱 | 캐릭터 이름 중복 검사 추가

return  : 0 = 성공
          1 = 이미 존재하는 캐릭터 이름
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_pc_create_insert]
    @p_user_no  INT
    , @p_pc_name NVARCHAR(20)
    , @p_pc_type TINYINT
    , @p_pc_no  BIGINT OUTPUT
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    DECLARE @a_error INT = 0;

    DECLARE @a_level INT = 1;
    DECLARE @a_exp   BIGINT = 0;
    DECLARE @a_hp    INT = 100;
    DECLARE @a_mp    INT = 100;

    SET @p_pc_no = 0;

    BEGIN TRAN;

    -- UPDLOCK, HOLDLOCK
    IF EXISTS (SELECT 1 FROM [dbo].[tb_pc] WITH (UPDLOCK, HOLDLOCK) WHERE pc_name = @p_pc_name)
    BEGIN
        ROLLBACK TRAN;
        SET @a_error = 1;
        RETURN @a_error;
    END

    INSERT INTO [dbo].[tb_pc] (
        [user_no],
        [pc_name],
        [pc_type],
        [level],
        [exp],
        [hp],
        [mp]
    )
    VALUES (
        @p_user_no,
        @p_pc_name,
        @p_pc_type,
        @a_level,
        @a_exp,
        @a_hp,
        @a_mp
    );
    
    SELECT @p_pc_no = SCOPE_IDENTITY();

    COMMIT TRAN;

    RETURN @a_error;
END
GO
// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MahjongSpirit.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, account(_T(""))
	, password(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ACCOUNT, account);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, password);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


void CLoginDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);	
	if (account == "Freezing" && password == "123456")
	{
		login_account = account;
		login_password = password;
		MessageBox(_T("登录成功！"));
		CDialogEx::OnOK();
	}
	else
	{
		MessageBox(_T("用户名或密码错误！"));
	}
}

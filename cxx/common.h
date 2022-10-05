#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>


const char ExecType_UNKOWN = 'a';
const char ExecType_EXPIRED = 'b';
const char ExecType_GET_ACCOUNTS = 'c';
const char ExecType_GET_ACCOUNT_BALANCES = 'd';
const char ExecType_GET_ACCOUNT_POSITIONS = 'e';

const char ExecType_GET_OPEN_ORDERS = 'f';
const char ExecType_RESPONSE_ERROR = 'g';
const char ExecType_REQUEST_FAILURE = 'h';

class ExecutionReport
  {
  public:
    // ExecutionReport() : Message(MsgType()) {}
    // ExecutionReport(const FIX::Message& m) : Message(m) {}
    // ExecutionReport(const Message& m) : Message(m) {}
    // ExecutionReport(const ExecutionReport& m) : Message(m) {}
    // static FIX::MsgType MsgType() { return FIX::MsgType("8"); }
    ExecutionReport() {}

    // ExecutionReport(
    //   const std::string& aOrderID,
    //   const std::string& aExecID,
    //   const std::string& aExecType,
    //   const int& aOrdStatus,
    //   const int& aSide,
    //   const double& aLeavesQty,
    //   const double& aCumQty)
    // : OrderID(aOrderID), ExecID(aExecID), ExecType(aExecType),OrdStatus(aOrdStatus),
    //   Side(aSide)/*,LeavesQty(aLeavesQty),CumQty(aCumQty)*/
    // {

    // }

    public:
      char ExecType;
      std::string Symbol;
      std::string OrderID;
      std::string ClOrdId;
      std::string ExecID;
      // std::string Instrument;


      double CumQty;  //Total quantity (e.g. number of shares) filled. //https://www.onixs.biz/fix-dictionary/4.4/tagNum_14.html
      double OrderQty; // TODO 这个后续核对fix对应字段的名称
      char OrdStatus;
      char Side;
      double AssetFree;
      double AssetLocked;


      // double LeavesQty;
      // double CumQty;

      //
      std::string ErrorMessage;
      int httpStatusCode;
    //   double CumulativeFilledPriceTimesQuantity;
    //   double CumulativeFilledQuantity;
      double LimitPrice;


  };


class NewOrderSingle  {
public:
    NewOrderSingle(
            const std::string &aSymbol,
            const std::string &aClOrdID,
            const char &aSide,
            const char &aOrdType,
            const double &aQuantity,
            const double &aPrice,
            const double &aStopPrice,
            const char &aTimeInForce) : Symbol(aSymbol), ClOrdID(aClOrdID), Side(aSide), OrdType(aOrdType),
            Quantity(aQuantity), Price(aPrice), StopPrice(aStopPrice), TimeInForce(aTimeInForce) {

    }
public:
    const std::string Symbol;
    const std::string ClOrdID;
    const std::string HandlInst; //还未使用
    const char Side;
    const std::string TransactTime; //还未使用
    const char OrdType;

    const double Quantity;

    const double Price;
    const double StopPrice;
    const char TimeInForce;
};


class OrderCancelRequest  {
public:
    OrderCancelRequest(
                const std::string aSymbol,
                const std::string aOrdID,
                const std::string aOrigClOrdID = "",
                const std::string aClOrdID = "",
                const char aSide = 0) : Symbol(aSymbol), OrdID(aOrdID), OrigClOrdID(aOrigClOrdID), ClOrdID(aClOrdID), Side(aSide) {

    }
public:
    const std::string Symbol;
    const std::string OrdID; // Either orderId or origClientOrderId must be sent. -- from binance api doc
    const std::string OrigClOrdID;
    const std::string ClOrdID;
    const char Side;
};



typedef  void (*FromAppCallback) (std::string message, std::string sessionId);
typedef  void (*FromAppCallbackExecutionReport) (const std::vector<ExecutionReport> &excutionReportList, std::string sessionId) ;

#endif
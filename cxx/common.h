#ifndef COMMON_H
#define COMMON_H
#include <string>




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
      std::string Symbol;
      std::string OrderID;
      std::string ClOrdId;
      std::string ExecID;
      std::string ExecType;
      char OrdStatus;
      int Side;
      // double LeavesQty;
      // double CumQty;

      std::string ErrorMessage;


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




typedef  void (*FromAppCallback) (std::string message, std::string sessionId);
typedef  void (*FromAppCallbackExecutionReport) (const std::vector<ExecutionReport> &excutionReportList, std::string sessionId) ;

#endif
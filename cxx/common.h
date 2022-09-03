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

    ExecutionReport(
      const std::string& aOrderID,
      const std::string& aExecID,
      const std::string& aExecType,
      const int& aOrdStatus,
      const int& aSide,
      const double& aLeavesQty,
      const double& aCumQty)
    : OrderID(aOrderID), ExecID(aExecID), ExecType(aExecType),OrdStatus(aOrdStatus),
      Side(aSide),LeavesQty(aLeavesQty),CumQty(aCumQty)
    {

    }

    public:
      std::string OrderID;
      std::string ExecID;
      std::string ExecType;
      int OrdStatus;
      int Side;
      double LeavesQty;
      double CumQty;


  };

typedef  void (*FromAppCallback) (std::string message, std::string sessionId);
typedef  void (*FromAppCallbackExecutionReport) (ExecutionReport executionReport) ;

#endif
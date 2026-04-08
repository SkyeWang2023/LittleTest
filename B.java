// 订单服务：计算最终折扣后价格
public class B {
    private C discountTool = new C();

    public double getFinalPrice(double originalPrice) {
        // 折扣金额
        double discount = discountTool.calculateDiscount(originalPrice, 0.2);
        // 正确逻辑：原价 - 折扣
        return originalPrice - discount;
    }
}

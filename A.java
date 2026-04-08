// 对外接口：给前端/用户展示最终价格
public class A {
    private B orderService = new B();

    public double showPrice(double price) {
        return orderService.getFinalPrice(price);
    }
}

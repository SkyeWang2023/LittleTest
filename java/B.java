// 中间层
public class B {
    private C c = new C();

    public int process(int x) {
        int value = c.calculate(x);
        // 逻辑依赖 C 的结果
        return value + 10;
    }
}

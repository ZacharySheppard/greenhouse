import { prisma } from "../lib/prisma";

const Posts = async () => {
  const posts = await prisma.post.findMany();
  return posts.map((post) => (
    <div key={post.title}>
      <h4>{post.title}</h4>
      <p>{post.content}</p>
    </div>
  ));
};

export default function Home() {
  return (
    <main>
      <div className="mark">✳</div>
      <p className="eyebrow">GREENHOUSE</p>
      <h1>A fresh start.</h1>
      <Posts />
    </main>
  );
}

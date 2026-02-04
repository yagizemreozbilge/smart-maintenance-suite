// Point 1: Your First Component (Functional, TitleCase)
// Point 2: Named Export
export function Header({ title }) {
    // Point 4: JavaScript in JSX using curly braces
    // Point 5: Passing Props (destructuring title from props object)
    return (
        <header className="main-header">
            <h1>{title}</h1>
        </header>
    );
}

// Point 2: Default Export
const WelcomeMessage = () => {
    return <p>Factory Maintenance Control Center</p>;
};

export default WelcomeMessage;
